#pragma once
/**
 * @file logger.hpp
 * @brief Public interface of logger class
 * @version 0.1
 * @date 2023-08-15
 * @copyright Copyright (c) 2023
 */

#include <string>
#include <fstream>
#include <thread>

#include "lfds/spscqueue.hpp"

#include "logger/log_element.hpp"

namespace logger {
    /**
     * @brief Logger class to log to a file.
     * Runs on a background thread for performance reasons.
     * Communicates with main threads via lock free SPSC queue - main threads
     * write data to the queue, logger reads and writes to a file.
     */
    class Logger final {
    private:
        // Keep track of the file name
        const std::string file_name_;

        // Log file stream that data is written to
        std::ofstream file_;

        // Queue log reads incoming messages from
        lfds::SPSCQueue<LogElement> queue_;
        
        // Controls lifetime of the background logging thread
        std::atomic<bool> running_ { true };

        // Background thread that writes to disk
        std::thread logger_thread_ {};

    public:
        /**
         * @brief Construct a new logger. Constructing immediately starts listening
         * on the queue for incoming messages and writing to the file specified by file_name.
         * 
         * @param file_name File to write logs to
         */
        explicit Logger(const std::string& file_name);

        /**
         * @brief Wait for the queue to be emptied by the background thread.
         * Then, set running to false and join background thread (wait until it finishes cleaning up).
         * Last, close the file and then return.
         */
        ~Logger();

        // Delete default/copy/move ctors, copy/move assigment
        Logger() = delete;

        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;

        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete; 

        /**
         * @brief Log a message to the logfile. Format string is printf style, but you don't specify the type after the %
         * For example: log("Hello %\n", "world") will log "Hello world" to the log file.
         * @note Remember to include newline character unless you specifically don't want it.
         * @tparam Args Argument types
         * @param str Format string.
         * @param args Arguments
         */
        template<typename First, typename... Args>
        void log(const char* str, const First& value, Args... args) noexcept {
          while (*str) {
            if (*str == '%') {
                if (*(str + 1) == '%') [[unlikely]] {
                    ++str;
                } else {
                    pushValue(value);
                    log(str + 1, args...);
                    return;
                }
            }
            pushValue(*str++);
          }
        }; 
        
        /**
         * @brief Log a message to the logfile. No format string version - simple strings.
         * @note Remember to include newline character unless you specifically don't want it.
         * @param str String to log to the logfile.
         */
        void log(const char* str) noexcept;

        /**
         * @brief Block calling thread until the queue is empty (meaning all log entries have been written to disk.)
         */
        void flushQueue() noexcept;

    private:
        /**
         * @brief Internal method that the background thread runs.
         * Spins in a loop as long as running_ is true, scanning queue for new logs and writing them to file.
         */
        void consumeQueue() noexcept;

        /**
         * @brief Internal method that logs an element - push a new log item to the queue.
         * Has convenience overloads that handle construction of LogElement
         * @param element Log element to push
         */
        void pushValue(const LogElement& element) noexcept;

        void pushValue(const char ch) noexcept;
        void pushValue(const char* cstr) noexcept;
        void pushValue(const std::string& str) noexcept;
        void pushValue(const int value) noexcept;
        void pushValue(const long value) noexcept;
        void pushValue(const long long value) noexcept;
        void pushValue(const unsigned value) noexcept;
        void pushValue(const unsigned long value) noexcept;
        void pushValue(const unsigned long long value) noexcept;
        void pushValue(const float value) noexcept;
        void pushValue(const double value) noexcept;
    };
}