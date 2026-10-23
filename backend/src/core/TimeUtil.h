#pragma once

#include <chrono>
#include <ctime>
#include <string>

namespace scenicflow {

inline std::tm toLocalTm(std::time_t t) {
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

// Formats a time_t with strftime. Returns "" when formatting fails.
inline std::string formatTime(std::time_t t, const char* fmt) {
    std::tm tm = toLocalTm(t);
    char buf[64] = {0};
    if (std::strftime(buf, sizeof(buf), fmt, &tm) == 0) return "";
    return std::string(buf);
}

// "YYYY-MM-DD HH:MM:SS" in local time.
inline std::string nowString() {
    return formatTime(std::time(nullptr), "%Y-%m-%d %H:%M:%S");
}

inline std::string timeStringMinutesAgo(int minutes) {
    return formatTime(std::time(nullptr) - static_cast<std::time_t>(minutes) * 60, "%Y-%m-%d %H:%M:%S");
}

inline std::string timeStringHoursAgo(int hours) {
    return formatTime(std::time(nullptr) - static_cast<std::time_t>(hours) * 3600, "%Y-%m-%d %H:%M:%S");
}

inline std::string dateStringDaysAgo(int days) {
    return formatTime(std::time(nullptr) - static_cast<std::time_t>(days) * 86400, "%Y-%m-%d");
}

inline long long nowMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// "YYYY-MM-DD HH:MM:SS" -> epoch milliseconds (0 when unparsable).
inline long long parseMillis(const std::string& s) {
    int y = 0, mo = 0, d = 0, h = 0, mi = 0, se = 0;
    if (std::sscanf(s.c_str(), "%d-%d-%d %d:%d:%d", &y, &mo, &d, &h, &mi, &se) != 6) return 0;
    std::tm tm{};
    tm.tm_year = y - 1900;
    tm.tm_mon = mo - 1;
    tm.tm_mday = d;
    tm.tm_hour = h;
    tm.tm_min = mi;
    tm.tm_sec = se;
    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    return static_cast<long long>(t) * 1000;
}

} // namespace scenicflow
