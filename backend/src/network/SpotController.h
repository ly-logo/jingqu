#pragma once

#include <drogon/HttpController.h>

#include <functional>

namespace scenicflow {

// Spot + real-time flow/congestion queries.
class SpotController : public drogon::HttpController<SpotController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SpotController::getSpots, "/api/spots", drogon::Get);
    ADD_METHOD_TO(SpotController::getFlow, "/api/spots/{spotId}/flow", drogon::Get);
    ADD_METHOD_TO(SpotController::getCongestion, "/api/spots/{spotId}/congestion", drogon::Get);
    METHOD_LIST_END

    void getSpots(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getFlow(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getCongestion(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace scenicflow
