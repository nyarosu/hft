#pragma once
/**
 * @file LogElement.hpp
 * @brief Struct that represents a single log element. 
 * @version 0.1
 * @date 2023-08-15
 * @copyright Copyright (c) 2023
 */

#include "logger/log_type.hpp"

namespace logger {
    /**
     * @brief One log element. Uses union to represent the fact that it
     * could contain any one of those types.
     */
    struct LogElement {
        LogType type_ { LogType::CHAR };

        // Element could be any of the following types
        union {
            char c;
            int i;
            long l;
            long long ll;
            unsigned u;
            unsigned long ul;
            unsigned long long ull;
            float f;
            double d;
        } u_;
    };
}