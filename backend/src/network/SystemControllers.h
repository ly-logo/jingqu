#pragma once

#include <drogon/HttpController.h>

#include <functional>

namespace scenicflow {

// GET /api/statistics?range=
class StatisticsController : public drogon::HttpController<StatisticsController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(StatisticsController::overview, "/api/statistics", drogon::Get);
    METHOD_LIST_END

    void overview(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// Simulation control: state / start / stop / config.
class SimulationController : public drogon::HttpController<SimulationController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SimulationController::state, "/api/simulation", drogon::Get);
    ADD_METHOD_TO(SimulationController::start, "/api/simulation/start", drogon::Post);
    ADD_METHOD_TO(SimulationController::stop, "/api/simulation/stop", drogon::Post);
    ADD_METHOD_TO(SimulationController::updateConfig, "/api/simulation/config", drogon::Put);
    METHOD_LIST_END

    void state(const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void start(const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void stop(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void updateConfig(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// GET /api/system/monitor
class MonitorController : public drogon::HttpController<MonitorController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(MonitorController::status, "/api/system/monitor", drogon::Get);
    METHOD_LIST_END

    void status(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// GET/PUT /api/settings
class SettingsController : public drogon::HttpController<SettingsController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SettingsController::get, "/api/settings", drogon::Get);
    ADD_METHOD_TO(SettingsController::update, "/api/settings", drogon::Put);
    METHOD_LIST_END

    void get(const drogon::HttpRequestPtr& req,
             std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void update(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

// Manual data entry (rejected while the simulation is running).
class EntryController : public drogon::HttpController<EntryController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(EntryController::visitor, "/api/entry/visitor", drogon::Post);
    ADD_METHOD_TO(EntryController::parking, "/api/entry/parking", drogon::Post);
    METHOD_LIST_END

    void visitor(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void parking(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

} // namespace scenicflow
