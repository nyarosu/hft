#pragma once
/**
 * @file assertions.h
 * @brief Useful assertions, that also perform logging
 * @version 1.0
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <string>
#include <iostream>
#include <cstdlib>

namespace utils {

    /**
     * @brief Asserts that condition is true, otherwise logs message and exits.
     *        Uses branch prediction hints.
     * @param condition Condition to be tested
     * @param message Message to log to cerr
     * @return void
     */
    inline void ASSERT(bool condition, const std::string& message) noexcept {
        if (!condition) [[unlikely]]
        {
            std::cerr << "Assertion failed: " << message << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Log error and exit immediately
     * @param message Message to log
     * @return void
     */
    inline void FATAL(const std::string& message) noexcept {
        std::cerr << "Fatal error: " << message << std::endl;
        exit(EXIT_FAILURE);
    }

}
