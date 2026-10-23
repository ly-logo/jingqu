#pragma once

#include <string>

namespace scenicflow {

// Business configuration loaded from config/app.json.
struct AppConfig {
    std::string dbPath = "data/scenicflow.db";
    std::string logDir = "logs";
    std::string logLevel = "INFO";

    int totalCapacity = 20000;
    int debounceSeconds = 30;
    int taskQueueMaxSize = 1000;
    int flowRecordRetentionDays = 7;
    int schedulingRecordRetentionDays = 30;
    int threadPoolSize = 4;
    int processWindowMs = 1000;

    int storageBatchSize = 100;
    int storageBatchIntervalMs = 1000;

    // Loads config/app.json. Missing keys keep their default values.
    static AppConfig load(const std::string& path);
};

// Global instance populated at startup by main().
inline AppConfig& appConfig() {
    static AppConfig cfg;
    return cfg;
}

} // namespace scenicflow
