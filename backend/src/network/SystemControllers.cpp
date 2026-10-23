#include "network/SystemControllers.h"

#include "core/Logger.h"
#include "core/Settings.h"
#include "core/TimeUtil.h"
#include "model/Enums.h"
#include "network/ApiResult.h"
#include "network/Mappers.h"
#include "pipeline/DataPipeline.h"
#include "pipeline/SimulationEngine.h"
#include "storage/Database.h"

#include <algorithm>
#include <cstdio>
#include <map>
#include <sstream>

namespace scenicflow {

namespace {

int parseRangeDays(const std::string& range) {
    if (range == "1d" || range == "today") return 1;
    if (range == "7d" || range == "week") return 7;
    if (range == "30d" || range == "month") return 30;
    if (!range.empty()) {
        try { return std::max(1, std::stoi(range)); } catch (...) {}
    }
    return 7;
}

Json::Value settingsToJson(const SystemSettings& s) {
    Json::Value j;
    j["totalCapacity"] = s.totalCapacity;
    j["debounceSeconds"] = s.debounceSeconds;
    j["retentionDays"] = s.retentionDays;
    j["batchIntervalMs"] = s.batchIntervalMs;
    return j;
}

Json::Value simulationStateToJson(bool running, const SimulationConfig& cfg) {
    Json::Value config;
    config["arrivalRate"] = cfg.arrivalRate;
    config["pattern"] = cfg.pattern;
    config["durationMinutes"] = cfg.durationMinutes;

    Json::Value j;
    j["running"] = running;
    j["config"] = config;
    return j;
}

bool isValidPattern(const std::string& p) {
    return p == "STEADY" || p == "PEAK" || p == "RANDOM";
}

} // namespace

// ==================== 统计 ====================

void StatisticsController::overview(const drogon::HttpRequestPtr& req,
                                    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    const int days = parseRangeDays(req->getParameter("range"));
    const std::string since = dateStringDaysAgo(days - 1) + " 00:00:00";

    // --- flow: daily arrivals derived from the cumulative counter delta ---
    auto flowRows = Database::instance().queryParams(
        "SELECT substr(record_time,1,10) AS d, spot_id, "
        "MAX(cumulative_visitors) AS mx, MIN(cumulative_visitors) AS mn "
        "FROM flow_record WHERE record_time >= ? GROUP BY d, spot_id",
        {since});
    std::map<std::string, int> daily;
    for (const auto& r : flowRows) {
        const int delta = toInt(field(r, "mx")) - toInt(field(r, "mn"));
        daily[field(r, "d")] += std::max(0, delta);
    }

    Json::Value flow(Json::arrayValue);
    for (int i = days - 1; i >= 0; --i) {
        const std::string date = dateStringDaysAgo(i);
        Json::Value point;
        point["date"] = date.substr(5);   // MM-DD, matches the frontend chart labels
        point["visitors"] = daily.count(date) ? daily[date] : 0;
        flow.append(point);
    }

    // --- congestion: minutes spent at CROWDED/OVERLOADED ---
    // The sampling interval differs between seeded history and live simulation records,
    // so the per-spot interval is derived from the actual sample count.
    auto congRows = Database::instance().queryParams(
        "SELECT s.name AS spot_name, "
        "SUM(CASE WHEN f.congestion_level IN ('CROWDED','OVERLOADED') THEN 1 ELSE 0 END) AS crowded, "
        "COUNT(*) AS total "
        "FROM flow_record f JOIN scenic_spot s ON s.spot_id=f.spot_id "
        "WHERE f.record_time >= ? GROUP BY f.spot_id ORDER BY crowded DESC",
        {since});
    const double rangeMinutes = days * 1440.0;
    Json::Value congestion(Json::arrayValue);
    for (const auto& r : congRows) {
        const int crowded = toInt(field(r, "crowded"));
        const int total = toInt(field(r, "total"));
        Json::Value item;
        item["spotName"] = field(r, "spot_name");
        item["crowdedMinutes"] =
            total > 0 ? static_cast<int>(std::lround(crowded * rangeMinutes / total)) : 0;
        congestion.append(item);
    }
    if (congestion.empty()) {
        auto spots = Database::instance().query("SELECT name FROM scenic_spot WHERE status=1 ORDER BY id");
        for (const auto& s : spots) {
            Json::Value item;
            item["spotName"] = field(s, "name");
            item["crowdedMinutes"] = 0;
            congestion.append(item);
        }
    }

    // --- resource: average utilisation per 2-hour bucket over the last 24h ---
    auto utilRows = Database::instance().queryParams(
        "SELECT substr(f.record_time,12,2) AS h, "
        "AVG(CAST(f.current_visitors AS REAL) / NULLIF(s.instant_capacity,0)) AS r "
        "FROM flow_record f JOIN scenic_spot s ON s.spot_id=f.spot_id "
        "WHERE f.record_time >= ? GROUP BY h",
        {formatTime(std::time(nullptr) - 86400, "%Y-%m-%d %H:%M:%S")});
    double buckets[12] = {0};
    int counts[12] = {0};
    for (const auto& r : utilRows) {
        int hour = toInt(field(r, "h"));
        if (hour < 0 || hour > 23) continue;
        const int idx = hour / 2;
        buckets[idx] += toDouble(field(r, "r")) * 100.0;
        ++counts[idx];
    }
    Json::Value resource(Json::arrayValue);
    for (int i = 0; i < 12; ++i) {
        char label[8];
        std::snprintf(label, sizeof(label), "%02d:00", i * 2);
        Json::Value item;
        item["time"] = std::string(label);
        item["utilization"] = counts[i] > 0 ? static_cast<int>(buckets[i] / counts[i] + 0.5) : 0;
        resource.append(item);
    }

    // --- suggestion type distribution ---
    auto suggRows = Database::instance().query(
        "SELECT schedule_type, COUNT(*) AS cnt FROM scheduling_record "
        "WHERE is_deleted=0 GROUP BY schedule_type");
    std::map<std::string, int> counts2;
    for (const auto& r : suggRows) counts2[field(r, "schedule_type")] = toInt(field(r, "cnt"));

    Json::Value typeCount(Json::arrayValue);
    for (const char* type : {"DIVERSION", "SHUTTLE_DISPATCH", "PARKING_GUIDE",
                             "ENTRANCE_LIMIT", "RESOURCE_WARNING"}) {
        Json::Value item;
        item["type"] = type;
        item["count"] = counts2.count(type) ? counts2[type] : 0;
        typeCount.append(item);
    }

    Json::Value data;
    data["flow"] = flow;
    data["congestion"] = congestion;
    data["resource"] = resource;
    data["suggestionTypeCount"] = typeCount;
    callback(ok(data));
}

// ==================== 模拟 ====================

void SimulationController::state(const drogon::HttpRequestPtr&,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto& engine = SimulationEngine::instance();
    callback(ok(simulationStateToJson(engine.isRunning(), engine.config())));
}

void SimulationController::start(const drogon::HttpRequestPtr&,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto& engine = SimulationEngine::instance();
    engine.start();
    callback(ok(simulationStateToJson(engine.isRunning(), engine.config())));
}

void SimulationController::stop(const drogon::HttpRequestPtr&,
                                std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto& engine = SimulationEngine::instance();
    engine.stop();
    callback(ok(simulationStateToJson(engine.isRunning(), engine.config())));
}

void SimulationController::updateConfig(const drogon::HttpRequestPtr& req,
                                        std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto body = req->getJsonObject();
    if (!body) {
        callback(failed(400, "请求体不是合法 JSON"));
        return;
    }

    auto& engine = SimulationEngine::instance();
    SimulationConfig cfg = engine.config();

    if ((*body).isMember("arrivalRate")) {
        const double rate = (*body)["arrivalRate"].asDouble();
        if (rate <= 0 || rate > 100000) {
            callback(failed(400, "arrivalRate 超出合法范围"));
            return;
        }
        cfg.arrivalRate = rate;
    }
    if ((*body).isMember("pattern")) {
        const std::string pattern = (*body)["pattern"].asString();
        if (!isValidPattern(pattern)) {
            callback(failed(400, "非法 pattern"));
            return;
        }
        cfg.pattern = pattern;
    }
    if ((*body).isMember("durationMinutes")) {
        const int duration = (*body)["durationMinutes"].asInt();
        if (duration <= 0) {
            callback(failed(400, "durationMinutes 必须大于 0"));
            return;
        }
        cfg.durationMinutes = duration;
    }

    engine.updateConfig(cfg);

    Json::Value data;
    data["arrivalRate"] = cfg.arrivalRate;
    data["pattern"] = cfg.pattern;
    data["durationMinutes"] = cfg.durationMinutes;
    callback(ok(data));
}

// ==================== 系统监控 ====================

void MonitorController::status(const drogon::HttpRequestPtr&,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    Json::Value pipelines(Json::arrayValue);
    for (const auto& p : DataPipeline::instance().statuses()) {
        Json::Value item;
        item["name"] = p.name;
        item["running"] = p.running;
        item["queueDepth"] = p.queueDepth;
        pipelines.append(item);
    }

    const bool simRunning = SimulationEngine::instance().isRunning();

    Json::Value sources(Json::arrayValue);
    auto addSource = [&sources](const std::string& name, const std::string& status) {
        Json::Value item;
        item["name"] = name;
        item["status"] = status;
        sources.append(item);
    };
    addSource("手动录入", simRunning ? "已暂停（模拟中）" : "正常");
    addSource("模拟生成器", simRunning ? "运行中" : "已停止");
    addSource("文件导入", "就绪");
    addSource("闸机接口", "未接入（预留）");
    addSource("停车场接口", "未接入（预留）");

    Json::Value logs(Json::arrayValue);
    auto recent = Logger::instance().recent(50);
    for (auto it = recent.rbegin(); it != recent.rend(); ++it) {
        Json::Value item;
        item["time"] = it->time;
        item["level"] = it->level;
        item["message"] = it->message;
        logs.append(item);
    }

    Json::Value data;
    data["pipelines"] = pipelines;
    data["sources"] = sources;
    data["logs"] = logs;
    callback(ok(data));
}

// ==================== 系统设置 ====================

void SettingsController::get(const drogon::HttpRequestPtr&,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    callback(ok(settingsToJson(currentSettings())));
}

void SettingsController::update(const drogon::HttpRequestPtr& req,
                                std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto body = req->getJsonObject();
    if (!body) {
        callback(failed(400, "请求体不是合法 JSON"));
        return;
    }

    SystemSettings s = currentSettings();
    if ((*body).isMember("totalCapacity")) {
        const int v = (*body)["totalCapacity"].asInt();
        if (v <= 0) { callback(failed(400, "totalCapacity 必须大于 0")); return; }
        s.totalCapacity = v;
    }
    if ((*body).isMember("debounceSeconds")) {
        const int v = (*body)["debounceSeconds"].asInt();
        if (v < 1 || v > 600) { callback(failed(400, "debounceSeconds 需在 1~600 之间")); return; }
        s.debounceSeconds = v;
    }
    if ((*body).isMember("retentionDays")) {
        const int v = (*body)["retentionDays"].asInt();
        if (v < 1 || v > 365) { callback(failed(400, "retentionDays 需在 1~365 之间")); return; }
        s.retentionDays = v;
    }
    if ((*body).isMember("batchIntervalMs")) {
        const int v = (*body)["batchIntervalMs"].asInt();
        if (v < 100 || v > 60000) { callback(failed(400, "batchIntervalMs 需在 100~60000 之间")); return; }
        s.batchIntervalMs = v;
    }

    saveSettings(s);
    callback(ok(settingsToJson(s)));
}

// ==================== 数据录入 ====================

void EntryController::visitor(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    if (SimulationEngine::instance().isRunning()) {
        callback(failed(409, "模拟模式下禁止手动修改游客数"));
        return;
    }

    auto body = req->getJsonObject();
    if (!body || !(*body).isMember("spotId") || !(*body).isMember("currentVisitors")) {
        callback(failed(400, "缺少 spotId 或 currentVisitors"));
        return;
    }

    const std::string spotId = (*body)["spotId"].asString();
    const int current = (*body)["currentVisitors"].asInt();
    if (current < 0) {
        callback(failed(400, "currentVisitors 不能为负数"));
        return;
    }

    auto rows = Database::instance().queryParams("SELECT * FROM scenic_spot WHERE spot_id=?", {spotId});
    if (rows.empty()) {
        callback(failed(404, "景点不存在"));
        return;
    }

    const int capacity = toInt(field(rows[0], "instant_capacity"));
    const double ratio = capacity > 0 ? static_cast<double>(current) / capacity : 0.0;

    Database::instance().execParams(
        "UPDATE scenic_spot SET current_visitors=?, updated_at=? WHERE spot_id=?",
        {std::to_string(current), nowString(), spotId});
    Database::instance().execParams(
        "INSERT INTO flow_record(record_time,spot_id,current_visitors,cumulative_visitors,"
        "congestion_level,arrival_rate,departure_rate,is_deleted) VALUES(?,?,?,?,?,0,0,0)",
        {nowString(), spotId, std::to_string(current),
         field(rows[0], "cumulative_visitors", "0"), toString(levelFromRatio(ratio))});

    logInfo("manual visitor entry: " + spotId + " -> " + std::to_string(current));
    callback(ok(Json::nullValue));
}

void EntryController::parking(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    if (SimulationEngine::instance().isRunning()) {
        callback(failed(409, "模拟模式下禁止手动修改停车场数据"));
        return;
    }

    auto body = req->getJsonObject();
    if (!body || !(*body).isMember("parkingId") || !(*body).isMember("occupiedSpaces")) {
        callback(failed(400, "缺少 parkingId 或 occupiedSpaces"));
        return;
    }

    const std::string parkingId = (*body)["parkingId"].asString();
    const int occupied = (*body)["occupiedSpaces"].asInt();

    auto rows = Database::instance().queryParams("SELECT * FROM parking_lot WHERE parking_id=?",
                                                 {parkingId});
    if (rows.empty()) {
        callback(failed(404, "停车场不存在"));
        return;
    }

    const int total = toInt(field(rows[0], "total_spaces"));
    if (occupied < 0 || occupied > total) {
        callback(failed(400, "occupiedSpaces 需在 0~" + std::to_string(total) + " 之间"));
        return;
    }

    Database::instance().execParams(
        "UPDATE parking_lot SET occupied_spaces=? WHERE parking_id=?",
        {std::to_string(occupied), parkingId});

    logInfo("manual parking entry: " + parkingId + " -> " + std::to_string(occupied));
    callback(ok(Json::nullValue));
}

} // namespace scenicflow
