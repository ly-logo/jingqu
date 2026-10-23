#include "network/SystemController.h"

#include "core/Config.h"
#include "network/ApiResult.h"
#include "network/Mappers.h"
#include "storage/Database.h"

namespace scenicflow {

namespace {

std::string cell(const std::vector<Row>& rows, const std::string& col) {
    if (rows.empty()) return "0";
    auto it = rows[0].find(col);
    return it == rows[0].end() ? "0" : it->second;
}

} // namespace

void SystemController::getHealth(const drogon::HttpRequestPtr&,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    Json::Value j;
    j["status"] = "UP";
    callback(ok(j));
}

void SystemController::getStatus(const drogon::HttpRequestPtr&,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto spotAgg = Database::instance().query(
        "SELECT COALESCE(SUM(current_visitors),0) AS cur, "
        "COALESCE(SUM(CASE WHEN instant_capacity>0 AND current_visitors*1.0/instant_capacity>=0.9 "
        "THEN 1 ELSE 0 END),0) AS crowded FROM scenic_spot WHERE status=1");
    auto parkAgg = Database::instance().query(
        "SELECT COALESCE(SUM(total_spaces-occupied_spaces),0) AS free FROM parking_lot");
    auto shuttleAgg = Database::instance().query(
        "SELECT COUNT(*) AS total, "
        "COALESCE(SUM(CASE WHEN status='RUNNING' THEN 1 ELSE 0 END),0) AS online FROM shuttle_bus");

    Json::Value j;
    j["totalCapacity"] = appConfig().totalCapacity;
    j["currentVisitors"] = toInt(cell(spotAgg, "cur"));
    j["crowdedSpotCount"] = toInt(cell(spotAgg, "crowded"));
    j["freeSpaces"] = toInt(cell(parkAgg, "free"));
    j["shuttleOnline"] = toInt(cell(shuttleAgg, "online"));
    j["shuttleTotal"] = toInt(cell(shuttleAgg, "total"));
    callback(ok(j));
}

} // namespace scenicflow
