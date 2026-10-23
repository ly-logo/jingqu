#pragma once

#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace scenicflow {

// Evaluates the configured rules against the current spot/parking/shuttle state.
// A rule must stay true for `debounceSeconds` before an event is raised, and an
// ACTIVE event is auto-resolved once its condition has been false for the same window.
class RuleEngine {
public:
    static RuleEngine& instance();

    // Called periodically by DataPipeline.
    void evaluate();

    size_t activeEventCount() const;

private:
    struct RuleRuntime {
        std::optional<std::chrono::steady_clock::time_point> trueSince;
    };

    mutable std::mutex m_mutex;
    std::unordered_map<std::string, RuleRuntime> m_runtime;     // ruleId:subject
    std::unordered_map<std::string, long long> m_activeEvents;  // type:spotId -> event id
    // Several rules may map to the same event key; an event is only auto-resolved
    // after every contributing rule has been false for the debounce window.
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_falseSince;

    bool evaluateCondition(const std::string& ruleId, const std::string& subject, bool condition,
                           const std::string& type, const std::string& level,
                           const std::string& spotId, const std::string& description,
                           const std::string& suggestion, int debounceSeconds);

    void raiseEvent(const std::string& key, const std::string& type, const std::string& level,
                    const std::string& spotId, const std::string& description,
                    const std::string& suggestion);
    void resolveEvent(const std::string& key, const std::string& type, const std::string& spotId);
};

} // namespace scenicflow
