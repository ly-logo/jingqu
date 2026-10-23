#pragma once

#include <drogon/HttpController.h>

#include <functional>

namespace scenicflow {

// System-level endpoints: health check and whole-scenic aggregate status.
class SystemController : public drogon::HttpController<SystemController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SystemController::getHealth, "/api/health", drogon::Get);
    ADD_METHOD_TO(SystemController::getStatus, "/api/system/status", drogon::Get);
    METHOD_LIST_END

    void getHealth(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getStatus(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace scenicflow
