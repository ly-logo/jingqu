#include "pipeline/RuleEngine.h"

#include "core/Logger.h"
#include "core/Settings.h"
#include "core/TimeUtil.h"
#include "network/Mappers.h"
#include "pipeline/SimulationEngine.h"
#include "storage/Database.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <vector>

namespace scenicflow {

namespace {

bool compare(double value, const std::string& op, double threshold) {
    if (op == ">")  return value > threshold;
    if (op == ">=") return value >= threshold;
    if (op == "<")  return value < threshold;
    if (op == "<=") return value <= threshold;
    return false;
}

std::string fmt1(double v) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(1);
    oss << v;
    return oss.str();
}

const char* eventTypeForMetric(const std::string& metric, double valueRatio) {
    if (metric == "visitor_ratio") return valueRatio >= 1.0 ? "OVERLOAD" : "CONGESTION";
    if (metric == "arrival_rate") return "DATA_ANOMALY";
    if (metric == "parking_free") return "PARKING_SHORTAGE";
    if (metric == "shuttle_load") return "CAPACITY_SHORTAGE";
    return "CONGESTION";
}

} // namespace

RuleEngine& RuleEngine::instance() {
    static RuleEngine inst;
    return inst;
}

size_t RuleEngine::activeEventCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeEvents.size();
}

void RuleEngine::evaluate() {
    auto ruleRows = Database::instance().query(
        "SELECT * FROM rule WHERE status=1 ORDER BY priority, id");
    if (ruleRows.empty()) return;

    const auto spots = SimulationEngine::instance().spots();
    auto parkingRows = Database::instance().query(
        "SELECT * FROM parking_lot WHERE status=1 ORDER BY id");
    auto shuttleRows = Database::instance().query("SELECT * FROM shuttle_bus ORDER BY id");

    int minFree = -1;
    std::string minFreeName;
    for (const auto& p : parkingRows) {
        const int total = toInt(field(p, "total_spaces"));
        const int occupied = toInt(field(p, "occupied_spaces"));
        const int free = total - occupied;
        if (minFree < 0 || free < minFree) {
            minFree = free;
            minFreeName = field(p, "name");
        }
    }

    double maxShuttleLoad = -1.0;
    std::string maxShuttleName;
    for (const auto& s : shuttleRows) {
        const int cap = toInt(field(s, "capacity"));
        if (cap <= 0) continue;
        const double load = static_cast<double>(toInt(field(s, "current_passengers"))) / cap;
        if (load > maxShuttleLoad) {
            maxShuttleLoad = load;
            maxShuttleName = field(s, "name");
        }
    }

    const int debounce = std::max(1, currentSettings().debounceSeconds);

    std::lock_guard<std::mutex> lock(m_mutex);
    std::set<std::string> hitKeys;

    for (const auto& rule : ruleRows) {
        const std::string ruleId = field(rule, "id");
        const std::string metric = field(rule, "metric");
        const std::string op = field(rule, "operator");
        const double threshold = toDouble(field(rule, "threshold"));
        const std::string level = field(rule, "level", "WARNING");
        const std::string action = field(rule, "action");
        const std::string targetSpot = field(rule, "spot_id", "*");

        if (metric == "parking_free") {
            if (minFree < 0) continue;
            const bool hit = compare(minFree, op, threshold);
            const std::string desc = minFreeName + "剩余车位 " + std::to_string(minFree) +
                                     " 个，" + op + " 阈值 " + fmt1(threshold);
            evaluateCondition(ruleId, "parking", hit, "PARKING_SHORTAGE", level, "",
                              desc, "建议引导车辆前往剩余车位最多的停车场", debounce);
            if (hit) hitKeys.insert("PARKING_SHORTAGE:");
            continue;
        }

        if (metric == "shuttle_load") {
            if (maxShuttleLoad < 0) continue;
            const bool hit = compare(maxShuttleLoad, op, threshold);
            const std::string desc = maxShuttleName + "满载率 " + fmt1(maxShuttleLoad * 100) +
                                     "%，" + op + " 阈值 " + fmt1(threshold * 100) + "%";
            evaluateCondition(ruleId, "shuttle", hit, "CAPACITY_SHORTAGE", level, "",
                              desc, "建议调派最近空闲摆渡车前往支援", debounce);
            if (hit) hitKeys.insert("CAPACITY_SHORTAGE:");
            continue;
        }

        for (const auto& s : spots) {
            if (targetSpot != "*" && targetSpot != s.spotId) continue;

            double value = 0.0;
            std::string desc;
            if (metric == "visitor_ratio") {
                value = s.ratio();
                desc = s.name + "当前在园 " + std::to_string(s.currentVisitors) + " 人，拥挤度 " +
                       fmt1(value * 100) + "%，" + op + " 阈值 " + fmt1(threshold * 100) + "%";
            } else if (metric == "arrival_rate") {
                value = s.arrivalRate;
                desc = s.name + "到达速率 " + fmt1(value) + " 人/分钟，" + op + " 阈值 " +
                       fmt1(threshold);
            } else {
                continue;
            }

            const bool hit = compare(value, op, threshold);
            const std::string type = eventTypeForMetric(metric, value);
            std::string suggestion = action;
            if (type == "CONGESTION" || type == "OVERLOAD") {
                suggestion = "建议将部分游客分流至容量充足的景点";
            }
            evaluateCondition(ruleId, s.spotId, hit, type, level, s.spotId, desc, suggestion,
                              debounce);
            if (hit) hitKeys.insert(type + ":" + s.spotId);
        }
    }

    // Auto-resolve only when no rule still holds the same event key.
    std::vector<std::string> toResolve;
    const auto now = std::chrono::steady_clock::now();
    for (const auto& entry : m_activeEvents) {
        const std::string& key = entry.first;
        if (hitKeys.count(key)) {
            m_falseSince.erase(key);
            continue;
        }
        auto it = m_falseSince.find(key);
        if (it == m_falseSince.end()) {
            m_falseSince[key] = now;
            continue;
        }
        if (std::chrono::duration<double>(now - it->second).count() >= debounce) {
            toResolve.push_back(key);
        }
    }
    for (const auto& key : toResolve) {
        const auto pos = key.find(':');
        const std::string type = pos == std::string::npos ? key : key.substr(0, pos);
        const std::string spotId = pos == std::string::npos ? "" : key.substr(pos + 1);
        resolveEvent(key, type, spotId);
        m_falseSince.erase(key);
    }
}

bool RuleEngine::evaluateCondition(const std::string& ruleId, const std::string& subject,
                                   bool condition, const std::string& type,
                                   const std::string& level, const std::string& spotId,
                                   const std::string& description,
                                   const std::string& suggestion, int debounceSeconds) {
    const std::string key = ruleId + ":" + subject;
    RuleRuntime& rt = m_runtime[key];
    const auto now = std::chrono::steady_clock::now();

    if (!condition) {
        rt.trueSince.reset();
        return false;
    }

    if (!rt.trueSince) rt.trueSince = now;
    const double seconds = std::chrono::duration<double>(now - *rt.trueSince).count();
    if (seconds >= debounceSeconds) {
        raiseEvent(type + ":" + spotId, type, level, spotId, description, suggestion);
    }
    return true;
}

void RuleEngine::raiseEvent(const std::string& key, const std::string& type,
                            const std::string& level, const std::string& spotId,
                            const std::string& description, const std::string& suggestion) {
    if (m_activeEvents.count(key)) {
        m_falseSince.erase(key);
        return;
    }

    // Survive restarts: reuse an ACTIVE event already present in the DB.
    auto existing = Database::instance().queryParams(
        "SELECT id FROM event WHERE event_type=? AND IFNULL(spot_id,'')=? AND status='ACTIVE'"
        " ORDER BY id DESC LIMIT 1",
        {type, spotId});
    if (!existing.empty()) {
        m_activeEvents[key] = toInt(existing[0]["id"]);
        m_falseSince.erase(key);
        return;
    }

    if (!Database::instance().execParams(
            "INSERT INTO event(event_type,occur_time,spot_id,level,description,suggestion,status)"
            " VALUES(?,?,?,?,?,?,'ACTIVE')",
            {type, nowString(), spotId, level, description, suggestion})) {
        logError("failed to insert event: " + Database::instance().lastError());
        return;
    }

    auto rows = Database::instance().queryParams(
        "SELECT id FROM event WHERE event_type=? AND IFNULL(spot_id,'')=? AND status='ACTIVE'"
        " ORDER BY id DESC LIMIT 1",
        {type, spotId});
    if (!rows.empty()) m_activeEvents[key] = toInt(rows[0]["id"]);
    m_falseSince.erase(key);
    logWarn("event raised: " + type + (spotId.empty() ? "" : " @" + spotId) + " - " + description);
}

void RuleEngine::resolveEvent(const std::string& key, const std::string& type,
                              const std::string& spotId) {
    Database::instance().execParams(
        "UPDATE event SET status='RESOLVED', updated_at=? "
        "WHERE event_type=? AND IFNULL(spot_id,'')=? AND status='ACTIVE'",
        {nowString(), type, spotId});
    m_activeEvents.erase(key);
    logInfo("event auto-resolved: " + type + (spotId.empty() ? "" : " @" + spotId));
}

} // namespace scenicflow
