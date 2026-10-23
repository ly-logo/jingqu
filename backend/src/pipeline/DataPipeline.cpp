#include "pipeline/DataPipeline.h"

#include "core/Logger.h"
#include "core/Settings.h"
#include "core/TimeUtil.h"
#include "pipeline/RuleEngine.h"
#include "pipeline/SchedulingEngine.h"
#include "pipeline/SimulationEngine.h"
#include "storage/Database.h"

#include <chrono>

namespace scenicflow {

namespace {

constexpr int kRuleIntervalSeconds = 3;
constexpr int kSchedulingIntervalSeconds = 5;
constexpr long long kCleanupIntervalMillis = 3600LL * 1000LL;

} // namespace

DataPipeline& DataPipeline::instance() {
    static DataPipeline inst;
    return inst;
}

void DataPipeline::start() {
    if (m_running) return;
    m_running = true;
    m_thread = std::thread(&DataPipeline::loop, this);
    logInfo("data pipeline started");
}

void DataPipeline::stop() {
    if (!m_running) return;
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
    logInfo("data pipeline stopped");
}

std::vector<PipelineStatus> DataPipeline::statuses() const {
    // The pipeline is a single orchestrated loop; each stage is reported individually so the
    // monitor page can show which part of the flow is currently active.
    const bool running = m_running.load();
    std::vector<PipelineStatus> out;
    out.push_back({"数据接入线程", running, 0});
    out.push_back({"预处理线程", running, 0});
    out.push_back({"拥挤计算线程", running, 0});
    out.push_back({"规则引擎线程", running,
                   static_cast<int>(RuleEngine::instance().activeEventCount())});
    out.push_back({"数据写库线程", running,
                   static_cast<int>(SchedulingEngine::instance().queueDepth())});
    return out;
}

void DataPipeline::runRetentionCleanup() {
    const SystemSettings cfg = currentSettings();

    // Flow records: hard delete beyond the retention window.
    Database::instance().execParams(
        "DELETE FROM flow_record WHERE record_time < ?",
        {formatTime(std::time(nullptr) - static_cast<std::time_t>(cfg.retentionDays) * 86400,
                    "%Y-%m-%d %H:%M:%S")});

    // Scheduling records: soft delete after 30 days (aggregated stats are kept).
    Database::instance().execParams(
        "UPDATE scheduling_record SET is_deleted=1 WHERE create_time < ? AND is_deleted=0",
        {formatTime(std::time(nullptr) - 30LL * 86400, "%Y-%m-%d %H:%M:%S")});

    logInfo("retention cleanup done (flow retention " + std::to_string(cfg.retentionDays) + "d)");
}

void DataPipeline::loop() {
    m_lastCleanupMillis = nowMillis();
    int lastRuleTick = -kRuleIntervalSeconds;
    int lastSchedulingTick = -kSchedulingIntervalSeconds;

    while (m_running) {
        const int tick = ++m_tickCount;

        try {
            SimulationEngine::instance().step();

            if (tick - lastRuleTick >= kRuleIntervalSeconds) {
                lastRuleTick = tick;
                RuleEngine::instance().evaluate();
            }

            if (tick - lastSchedulingTick >= kSchedulingIntervalSeconds) {
                lastSchedulingTick = tick;
                SchedulingEngine::instance().evaluate();
            }

            if (nowMillis() - m_lastCleanupMillis >= kCleanupIntervalMillis) {
                m_lastCleanupMillis = nowMillis();
                runRetentionCleanup();
            }
        } catch (const std::exception& e) {
            logError(std::string("data pipeline tick failed: ") + e.what());
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace scenicflow
