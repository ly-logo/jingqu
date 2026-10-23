#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "pipeline/SimulationEngine.h"
#include "pipeline/ThreadSafeQueue.h"

namespace scenicflow {

struct ShuttleLoad {
    std::string shuttleId;
    std::string name;
    std::string station;
    double load = 0.0;
    std::string status;
};

struct SchedulingSnapshot {
    std::vector<SpotState> spots;
    std::vector<ShuttleLoad> shuttles;
    int totalCapacity = 0;
    int totalVisitors = 0;
    int totalParkingSpaces = 0;
    int totalParkingFree = 0;
    std::string bestParkingName;
    int bestParkingFree = 0;
};

// Resource allocation / scheduling suggestions (M4).
// Snapshots are pushed into a thread-safe bounded queue and consumed by one worker thread,
// so the 1s data pipeline is never blocked by suggestion generation.
class SchedulingEngine {
public:
    static SchedulingEngine& instance();

    void start();
    void stop();

    // Enqueues the current state for the worker. Drops the snapshot if the queue is full.
    void evaluate();

    size_t queueDepth() const;

private:
    void workerLoop();
    void generate(const SchedulingSnapshot& snap);
    bool recentlySuggested(const std::string& type, const std::string& spotId, int minutes);
    void insertSuggestion(const std::string& type, const std::string& content,
                          const std::string& detail, const std::string& spotId);

    ThreadSafeQueue<SchedulingSnapshot> m_queue{200};
    std::thread m_worker;
    std::atomic<bool> m_running{false};
    mutable std::mutex m_startMutex;
};

} // namespace scenicflow
