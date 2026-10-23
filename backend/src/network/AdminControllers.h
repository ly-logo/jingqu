#pragma once

#include <drogon/HttpController.h>

#include <functional>

namespace scenicflow {

// Event center: list with filters + status transitions.
class EventController : public drogon::HttpController<EventController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(EventController::list, "/api/events", drogon::Get);
    ADD_METHOD_TO(EventController::updateStatus, "/api/events/{eventId}/status", drogon::Put);
    METHOD_LIST_END

    void list(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void updateStatus(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// Scheduling suggestions: list / detail / status update.
class SchedulingController : public drogon::HttpController<SchedulingController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SchedulingController::list, "/api/scheduling", drogon::Get);
    ADD_METHOD_TO(SchedulingController::detail, "/api/scheduling/{suggestionId}", drogon::Get);
    ADD_METHOD_TO(SchedulingController::updateStatus, "/api/scheduling/{suggestionId}/status",
                  drogon::Put);
    METHOD_LIST_END

    void list(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void detail(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void updateStatus(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// Rule configuration CRUD.
class RuleController : public drogon::HttpController<RuleController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RuleController::list, "/api/rules", drogon::Get);
    ADD_METHOD_TO(RuleController::create, "/api/rules", drogon::Post);
    ADD_METHOD_TO(RuleController::update, "/api/rules/{ruleId}", drogon::Put);
    ADD_METHOD_TO(RuleController::remove, "/api/rules/{ruleId}", drogon::Delete);
    METHOD_LIST_END

    void list(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void create(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void update(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void remove(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// User management (admin only, enforced by the auth advice for non-GET methods).
class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserController::list, "/api/users", drogon::Get);
    ADD_METHOD_TO(UserController::create, "/api/users", drogon::Post);
    ADD_METHOD_TO(UserController::remove, "/api/users/{userId}", drogon::Delete);
    METHOD_LIST_END

    void list(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void create(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void remove(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace scenicflow
