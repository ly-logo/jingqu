#include "network/ResourceController.h"

#include "network/ApiResult.h"
#include "network/Mappers.h"
#include "storage/Database.h"

namespace scenicflow {

void ResourceController::getParking(const drogon::HttpRequestPtr&,
                                    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto rows = Database::instance().query("SELECT * FROM parking_lot WHERE status=1 ORDER BY id");
    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows) arr.append(parkingToJson(r));
    callback(ok(arr));
}

void ResourceController::getShuttles(const drogon::HttpRequestPtr&,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto rows = Database::instance().query("SELECT * FROM shuttle_bus ORDER BY id");
    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows) arr.append(shuttleToJson(r));
    callback(ok(arr));
}

} // namespace scenicflow
