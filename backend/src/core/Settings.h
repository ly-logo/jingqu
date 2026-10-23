#pragma once

#include <mutex>
#include <string>

namespace scenicflow {

// Runtime-tunable settings exposed by GET/PUT /api/settings.
// Persisted in the system_config key-value table so they survive restarts.
struct SystemSettings {
    int totalCapacity = 43000;
    int debounceSeconds = 30;
    int retentionDays = 7;
    int batchIntervalMs = 1000;
};

// Loads settings from system_config (falls back to defaults, seeding missing keys).
SystemSettings loadSettings();

// Persists every field and refreshes the in-memory cached copy.
bool saveSettings(const SystemSettings& s);

// Cached copy used by the pipeline threads.
SystemSettings currentSettings();
void setCachedSettings(const SystemSettings& s);

} // namespace scenicflow
