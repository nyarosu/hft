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
         * @brief Log an element - push a new log item to the queue.
         * In most cases, you should be using Logger::log() instead of this method.
         * @param element Log element to push
         */
        void pushValue(const LogElement& element) noexcept;

        /**
         * @brief Log a character - push a character to the logging queue.
         * Internally, just constructs a LogElement and calls other overload.
         * @param ch Character to log 
         */
        void pushValue(const char ch) noexcept;

        /**
         * @brief Log a c-style string - push all the characters of the string to the logging queue
         * @param cstr C-style string you want logged
         * @todo implementation can be made faster by using a memcpy instead of a loop
         */ 
        void pushValue(const char* cstr) noexcept;

        /**
         * @brief Log a string - push all the characters of the string to the logging queue.
         * Internally, simply converts the string to a c-style string and calls other overload.
         * @param str String to log
         */
        void pushValue(const std::string& str) noexcept;

    private:
        /**
         * @brief Internal method that the background thread runs.
         * Spins in a loop as long as running_ is true, scanning queue for new logs and writing them to file.
         */
        void consumeQueue() noexcept;
    };
}