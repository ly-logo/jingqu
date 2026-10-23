#include "core/Settings.h"

#include "core/Logger.h"
#include "storage/Database.h"

#include <cstdlib>

namespace scenicflow {

namespace {

std::mutex g_mutex;
SystemSettings g_cached;

std::string getValue(const std::string& key, const std::string& def) {
    auto rows = Database::instance().queryParams("SELECT value FROM system_config WHERE key=?", {key});
    if (rows.empty()) return def;
    return rows[0]["value"];
}

void putValue(const std::string& key, const std::string& value) {
    Database::instance().execParams(
        "INSERT INTO system_config(key,value) VALUES(?,?) "
        "ON CONFLICT(key) DO UPDATE SET value=excluded.value",
        {key, value});
}

int toIntOr(const std::string& s, int def) {
    try { return std::stoi(s); } catch (...) { return def; }
}

} // namespace

SystemSettings loadSettings() {
    SystemSettings s;
    s.totalCapacity = toIntOr(getValue("totalCapacity", "43000"), 43000);
    s.debounceSeconds = toIntOr(getValue("debounceSeconds", "30"), 30);
    s.retentionDays = toIntOr(getValue("retentionDays", "7"), 7);
    s.batchIntervalMs = toIntOr(getValue("batchIntervalMs", "1000"), 1000);

    // Seed any missing keys so the table is complete after the first run.
    putValue("totalCapacity", std::to_string(s.totalCapacity));
    putValue("debounceSeconds", std::to_string(s.debounceSeconds));
    putValue("retentionDays", std::to_string(s.retentionDays));
    putValue("batchIntervalMs", std::to_string(s.batchIntervalMs));
    putValue("simArrivalRate", getValue("simArrivalRate", "45"));
    putValue("simPattern", getValue("simPattern", "PEAK"));
    putValue("simDurationMinutes", getValue("simDurationMinutes", "120"));

    setCachedSettings(s);
    logInfo("settings loaded (totalCapacity=" + std::to_string(s.totalCapacity) +
            ", debounce=" + std::to_string(s.debounceSeconds) + "s)");
    return s;
}

bool saveSettings(const SystemSettings& s) {
    putValue("totalCapacity", std::to_string(s.totalCapacity));
    putValue("debounceSeconds", std::to_string(s.debounceSeconds));
    putValue("retentionDays", std::to_string(s.retentionDays));
    putValue("batchIntervalMs", std::to_string(s.batchIntervalMs));
    setCachedSettings(s);
    logInfo("settings updated by admin");
    return true;
}

SystemSettings currentSettings() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_cached;
}

void setCachedSettings(const SystemSettings& s) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_cached = s;
}

} // namespace scenicflow
