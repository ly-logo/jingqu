#include "core/Config.h"

#include <fstream>
#include <json/json.h>

namespace scenicflow {

AppConfig AppConfig::load(const std::string& path) {
    AppConfig cfg;
    std::ifstream in(path);
    if (!in.is_open()) {
        return cfg; // fall back to defaults
    }
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    if (!Json::parseFromStream(builder, in, &root, &errs)) {
        return cfg;
    }

    auto str = [&](const char* key, std::string& out) {
        if (root.isMember(key) && root[key].isString()) out = root[key].asString();
    };
    auto num = [&](const char* key, int& out) {
        if (root.isMember(key) && root[key].isInt()) out = root[key].asInt();
    };

    str("dbPath", cfg.dbPath);
    str("logDir", cfg.logDir);
    str("logLevel", cfg.logLevel);

    num("totalCapacity", cfg.totalCapacity);
    num("debounceSeconds", cfg.debounceSeconds);
    num("taskQueueMaxSize", cfg.taskQueueMaxSize);
    num("flowRecordRetentionDays", cfg.flowRecordRetentionDays);
    num("schedulingRecordRetentionDays", cfg.schedulingRecordRetentionDays);
    num("threadPoolSize", cfg.threadPoolSize);
    num("processWindowMs", cfg.processWindowMs);
    num("storageBatchSize", cfg.storageBatchSize);
    num("storageBatchIntervalMs", cfg.storageBatchIntervalMs);

    return cfg;
}

} // namespace scenicflow
