#include "core/Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

namespace scenicflow {

namespace {

std::string nowString() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string dateString() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

const char* levelName(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARNING";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
    }
    return "INFO";
}

const char* levelShort(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "ERROR";
    }
    return "INFO";
}

LogLevel parseLevel(const std::string& s) {
    if (s == "DEBUG") return LogLevel::Debug;
    if (s == "WARNING" || s == "WARN") return LogLevel::Warning;
    if (s == "ERROR") return LogLevel::Error;
    if (s == "CRITICAL") return LogLevel::Critical;
    return LogLevel::Info;
}

} // namespace

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::init(const std::string& logDir, const std::string& level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLevel = parseLevel(level);
    std::string dir = logDir.empty() ? "logs" : logDir;
    std::string file = dir + "/scenicflow-" + dateString() + ".log";
    m_file.open(file, std::ios::app);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < m_minLevel) return;
    std::string ts = nowString();
    std::ostringstream line;
    line << "[" << ts << "] [" << levelName(level) << "] " << message;

    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << line.str() << std::endl;
    if (m_file.is_open()) {
        m_file << line.str() << std::endl;
        m_file.flush();
    }
    m_recent.push_back({ts, levelShort(level), message});
    if (m_recent.size() > 200) {
        m_recent.erase(m_recent.begin(), m_recent.begin() + (m_recent.size() - 200));
    }
}

std::vector<Logger::Entry> Logger::recent(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Entry> out;
    size_t start = m_recent.size() > maxCount ? m_recent.size() - maxCount : 0;
    for (size_t i = start; i < m_recent.size(); ++i) {
        out.push_back(m_recent[i]);
    }
    return out;
}

void logDebug(const std::string& msg)    { Logger::instance().log(LogLevel::Debug, msg); }
void logInfo(const std::string& msg)     { Logger::instance().log(LogLevel::Info, msg); }
void logWarn(const std::string& msg)     { Logger::instance().log(LogLevel::Warning, msg); }
void logError(const std::string& msg)    { Logger::instance().log(LogLevel::Error, msg); }
void logCritical(const std::string& msg) { Logger::instance().log(LogLevel::Critical, msg); }

} // namespace scenicflow
