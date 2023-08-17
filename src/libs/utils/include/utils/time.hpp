#pragma once
/**
 * @file time.hpp
 * @brief Time utils to get time in various formats. Used for logging, performance profiling, etc.
 * @version 0.1
 * @copyright Copyright (c) 2023
 */

#include <chrono>
#include <ctime>
#include <string>

namespace utils {
    using Nanos = int64_t;
    
    constexpr Nanos NANOS_TO_MICROS { 1000 };
    constexpr Nanos MICROS_TO_MILLIS { 1000 };
    constexpr Nanos MILLIS_TO_SECS { 1000 };
    constexpr Nanos NANOS_TO_MILLIS { NANOS_TO_MICROS * MICROS_TO_MILLIS };
    constexpr Nanos NANOS_TO_SECS { NANOS_TO_MILLIS * MILLIS_TO_SECS };

    /**
     * @brief Current time, since epoch, in nanoseconds. 
     * @return current time in ns 
     */
    inline auto getCurrentNanos() noexcept {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    /**
     * @brief Get the Current Time Str object
     * 
     * @param time_str Sets this string to the current time in string format
     * @return auto& reference to the string
     */
    inline std::string& getCurrentTimeStr(std::string& time_str) {
        const auto time { std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
        time_str.assign(ctime(&time));

        if (!time_str.empty()) {
            time_str.at(time_str.length() - 1) = '\0';
        }

        return time_str;
    }
    
}