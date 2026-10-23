#pragma once

#include <string>

namespace scenicflow {

enum class CongestionLevel { EMPTY, NORMAL, MODERATE, CROWDED, OVERLOADED };

inline std::string toString(CongestionLevel level) {
    switch (level) {
        case CongestionLevel::EMPTY:      return "EMPTY";
        case CongestionLevel::NORMAL:     return "NORMAL";
        case CongestionLevel::MODERATE:   return "MODERATE";
        case CongestionLevel::CROWDED:    return "CROWDED";
        case CongestionLevel::OVERLOADED: return "OVERLOADED";
    }
    return "NORMAL";
}

// Ratio -> congestion level. Mirrors frontend utils/level.ts (0.3 / 0.6 / 0.9 / 1.0 boundaries).
inline CongestionLevel levelFromRatio(double ratio) {
    if (ratio < 0.3) return CongestionLevel::EMPTY;
    if (ratio < 0.6) return CongestionLevel::NORMAL;
    if (ratio < 0.9) return CongestionLevel::MODERATE;
    if (ratio < 1.0) return CongestionLevel::CROWDED;
    return CongestionLevel::OVERLOADED;
}

} // namespace scenicflow
