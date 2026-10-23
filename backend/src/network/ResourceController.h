#pragma once

#include <drogon/HttpController.h>

#include <functional>

namespace scenicflow {

// Parking lot and shuttle bus read-only queries.
class ResourceController : public drogon::HttpController<ResourceController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ResourceController::getParking, "/api/parking", drogon::Get);
    ADD_METHOD_TO(ResourceController::getShuttles, "/api/shuttles", drogon::Get);
    METHOD_LIST_END

    void getParking(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getShuttles(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace scenicflow
