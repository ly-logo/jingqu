#pragma once

#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace scenicflow {

// Enumerator names are CamelCase on purpose: the Windows SDK (wingdi.h, pulled in
// transitively by drogon/trantor) defines ERROR and WARNING as object-like macros,
// which would corrupt SCREAMING_CASE enumerators when Logger.h is included after it.
enum class LogLevel { Debug = 0, Info = 1, Warning = 2, Error = 3, Critical = 4 };

// Thread-safe logger: writes to stdout and to logs/scenicflow-YYYYMMDD.log.
// Also keeps the most recent entries in memory so /api/system/monitor can return them.
class Logger {
public:
    static Logger& instance();

    void init(const std::string& logDir, const std::string& level);

    void log(LogLevel level, const std::string& message);

    // Recent log entries as JSON-ready plain structs (for the monitor endpoint).
    struct Entry {
        std::string time;
        std::string level;   // INFO / WARN / ERROR
        std::string message;
    };
    std::vector<Entry> recent(size_t maxCount) const;

private:
    Logger() = default;

    mutable std::mutex m_mutex;
    std::ofstream m_file;
    LogLevel m_minLevel = LogLevel::Info;
    std::vector<Entry> m_recent;
};

void logDebug(const std::string& msg);
void logInfo(const std::string& msg);
void logWarn(const std::string& msg);
void logError(const std::string& msg);
void logCritical(const std::string& msg);

} // namespace scenicflow
