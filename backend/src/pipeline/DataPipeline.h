#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace scenicflow {

struct PipelineStatus {
    std::string name;
    bool running = false;
    int queueDepth = 0;
};

// Orchestrates the 1s data pipeline:
//   simulation source -> congestion calc (level derived on read) -> rule engine -> scheduling engine
// Also owns the periodic data-retention cleanup (soft delete + aggregation policy).
class DataPipeline {
public:
    static DataPipeline& instance();

    void start();
    void stop();

    std::vector<PipelineStatus> statuses() const;

    // Deletes expired flow records and old scheduling records.
    void runRetentionCleanup();

private:
    void loop();

    std::thread m_thread;
    std::atomic<bool> m_running{false};
    std::atomic<int> m_tickCount{0};
    long long m_lastCleanupMillis = 0;
};

} // namespace scenicflow
