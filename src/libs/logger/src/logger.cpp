/**
 * @file logger.cpp
 * @brief Implementation of logger class
 * @version 0.1
 * @copyright Copyright (c) 2023
 */

#include "logger/logger.hpp"
#include "constants.hpp"
#include "logger/log_type.hpp"
#include "utils/assertions.hpp"
#include "utils/threads/threads.hpp"

namespace logger {
    Logger::Logger(const std::string& file_name)
        : file_name_ { file_name }, queue_ { LOG_QUEUE_SIZE } 
    {
        // Attempt to load provided file name, triggering an assertion error if it fails
        file_.open(file_name, std::ios::out | std::ios::app);
        utils::ASSERT(file_.is_open(), "Failed to open log file: " + file_name);

        // Spawn background thread that will handle writes. -1 affinity indicates not to set affinity - this is a low priority, background thread.
        logger_thread_ = utils::threads::createAndStart(-1, "Logger", [this]() { consumeQueue(); });

        // If thread creation fails, it is joined before being returned - meaning we can check for joinability here as a failure check.
        utils::ASSERT(logger_thread_.joinable(), "Failed to start logging thread");
    }

    Logger::~Logger() {
        // Wait for queue to empty
        while (queue_.size()) {
            std::this_thread::sleep_for(std::chrono::seconds { 1 });
        }
        // Set running to false and wait for thread to finish up
        running_ = false;
        logger_thread_.join();

        file_.close();
    }

    void Logger::setPrefix(const std::string& prefix) noexcept {
        prefix_ = prefix;
    }

    void Logger::clearPrefix() noexcept {
        prefix_ = "";
    }

    void Logger::log(const char* str) noexcept  {
        // If prefix is set, write that first
        if (!prefix_.empty()) {
            pushValue(prefix_);
        }
            while (*str) {
                if (*str == '%') {
                    if (*(str + 1) == '%') [[unlikely]] {
                        ++str;
                    } else {
                        return;
                    }
                }
                pushValue(*str++);
            }
        }

    void Logger::consumeQueue() noexcept {
        while (running_) {
            for (auto next { queue_.getNextRead() }; queue_.size() && next; next = queue_.getNextRead()) {
                switch (next->type_) {
                    case LogType::CHAR:
                        file_ << next->u_.c; break;
                    case LogType::INTEGER:
                        file_ << next->u_.i; break;
                    case LogType::LONG_INTEGER:
                        file_ << next->u_.l; break;
                    case LogType::LONG_LONG_INTEGER:
                        file_ << next->u_.ll; break;
                    case LogType::UNSIGNED_INTEGER:
                        file_ << next->u_.u; break;
                    case LogType::UNSIGNED_LONG_INTEGER:
                        file_ << next->u_.ul; break;
                    case LogType::UNSIGNED_LONG_LONG_INTEGER:
                        file_ << next->u_.ull; break;
                    case LogType::FLOAT: 
                        file_ << next->u_.f; break;
                    case LogType::DOUBLE: 
                        file_ << next->u_.d; break;
                }
                // Done processing this element, update read index
                queue_.updateReadIndex();
                next = queue_.getNextRead();
            }
            // Ran out of elements, let's wait before checking again.
            std::this_thread::sleep_for(std::chrono::milliseconds { 1 });
        }
    }

    void Logger::flushQueue() noexcept {
        while (queue_.size()) {
            std::this_thread::sleep_for(std::chrono::seconds { 1 });
        }
        file_.flush();
    }
    
    void Logger::pushValue(const LogElement& element) noexcept {
        *(queue_.getNextWriteTo()) = element;
        queue_.updateWriteIndex();
    }

    void Logger::pushValue(const char ch) noexcept {
        pushValue(LogElement { LogType::CHAR, { .c = ch } });
    }

    void Logger::pushValue(const char* cstr) noexcept {
        while (*cstr) {
            pushValue(*cstr);
            ++cstr;
        }
    }

    void Logger::pushValue(const std::string& str) noexcept {
        pushValue(str.c_str());
    }

    void Logger::pushValue(const int value) noexcept {
        pushValue(LogElement{LogType::INTEGER, {.i = value}});
    }   

    void Logger::pushValue(const long value) noexcept {
        pushValue(LogElement{LogType::LONG_INTEGER, {.l = value}});
    }

    void Logger::pushValue(const long long value) noexcept {
        pushValue(LogElement{LogType::LONG_LONG_INTEGER, {.ll = value}});
    }

    void Logger::pushValue(const unsigned value) noexcept {
        pushValue(LogElement{LogType::UNSIGNED_INTEGER, {.u = value}});
    }

    void Logger::pushValue(const unsigned long value) noexcept {
        pushValue(LogElement{LogType::UNSIGNED_LONG_INTEGER, {.ul = value}});
    }

    void Logger::pushValue(const unsigned long long value) noexcept {
        pushValue(LogElement{LogType::UNSIGNED_LONG_LONG_INTEGER, {.ull = value}});
    }

    void Logger::pushValue(const float value) noexcept {
        pushValue(LogElement{LogType::FLOAT, {.f = value}});
    }

    void Logger::pushValue(const double value) noexcept {
        pushValue(LogElement{LogType::DOUBLE, {.d = value}});
    }
}