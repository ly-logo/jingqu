#pragma once

#include <drogon/HttpController.h>

#include <functional>

namespace scenicflow {

// Read-only aggregated endpoints for the visitor-facing side
// (requirement 13.2 / 23.2). Every numeric visitor figure is paired with a
// plain-language verdict (拥挤 / 适中 / 空闲); operational metrics are
// deliberately not exposed here.
class UserSideController : public drogon::HttpController<UserSideController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserSideController::overview, "/api/user/overview", drogon::Get);
    ADD_METHOD_TO(UserSideController::rankings, "/api/user/rankings", drogon::Get);
    ADD_METHOD_TO(UserSideController::forecast, "/api/user/forecast", drogon::Get);
    ADD_METHOD_TO(UserSideController::parking, "/api/user/parking", drogon::Get);
    ADD_METHOD_TO(UserSideController::shuttleRoutes, "/api/user/shuttle-routes", drogon::Get);
    ADD_METHOD_TO(UserSideController::pois, "/api/user/pois", drogon::Get);
    ADD_METHOD_TO(UserSideController::recommendRoutes, "/api/user/routes/recommend", drogon::Get);
    ADD_METHOD_TO(UserSideController::spotDetail, "/api/spots/{spotId}/detail", drogon::Get);
    METHOD_LIST_END

    void overview(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void rankings(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void forecast(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void parking(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void shuttleRoutes(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void pois(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void recommendRoutes(const drogon::HttpRequestPtr& req,
                         std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void spotDetail(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace scenicflow
