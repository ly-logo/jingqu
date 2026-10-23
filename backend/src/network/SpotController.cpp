#include "network/SpotController.h"

#include "core/TimeUtil.h"
#include "network/ApiResult.h"
#include "network/Mappers.h"
#include "network/RequestUtil.h"
#include "storage/Database.h"

namespace scenicflow {

void SpotController::getSpots(const drogon::HttpRequestPtr&,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto rows = Database::instance().query("SELECT * FROM scenic_spot WHERE status=1 ORDER BY id");
    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows) {
        arr.append(spotToJson(r));
    }
    callback(ok(arr));
}

void SpotController::getFlow(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    const std::string spotId = pathParam(req);
    // Newest 300 points, re-sorted ascending for the chart.
    auto rows = Database::instance().queryParams(
        "SELECT record_time, current_visitors FROM ("
        "  SELECT record_time, current_visitors FROM flow_record"
        "  WHERE spot_id=? AND is_deleted=0 ORDER BY record_time DESC LIMIT 300"
        ") ORDER BY record_time ASC",
        {spotId});

    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows) {
        Json::Value p;
        p["timestamp"] = static_cast<Json::Int64>(parseMillis(field(r, "record_time")));
        p["visitors"] = toInt(field(r, "current_visitors"));
        arr.append(p);
    }
    callback(ok(arr));
}

void SpotController::getCongestion(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    const std::string spotId = pathParam(req);
    auto rows = Database::instance().queryParams("SELECT * FROM scenic_spot WHERE spot_id=?", {spotId});
    if (rows.empty()) {
        callback(failed(404, "景点不存在"));
        return;
    }
    callback(ok(spotToJson(rows[0])));
}

} // namespace scenicflow
