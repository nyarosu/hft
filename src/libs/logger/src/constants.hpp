#pragma once
/**
 * @file constants.hpp
 * @brief Constants useful for logging
 * @version 0.1
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstdio>

namespace logger {
    constexpr size_t LOG_QUEUE_SIZE { 8 * 1024 * 1024 };
}