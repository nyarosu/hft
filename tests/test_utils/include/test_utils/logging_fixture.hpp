#pragma once
/**
 * @file logger_fixture.hpp
 * @brief Test fixture (common setup/teardown) for logger tests
 * @date 2023-08-18
 * @copyright Copyright (c) 2023
 */

#include <gtest/gtest.h>
#include "logger/logger.hpp"

class LoggingFixture: public ::testing::Test {
    protected:
        /**
         * @brief Create logger object
         */
        void SetUp() override;

        /**
         * @brief Destroy logger object as well as log file that was created
         */
        void TearDown() override;
        
        /**
         * @brief Get the log file name being used.
         * @return std::string file name
         */
        std::string getLogFileName() const { return logFileName_; }

        /**
         * @brief Ptr to logger object to be used in tests
         */
        std::unique_ptr<logger::Logger> logger_ { nullptr };

        /**
         * @brief Name of the log file to be used in tests
         * Format: log_<random_number>.log
         */
        std::string logFileName_;

    private:
        /**
         * @brief Create a unique log file name 
         * @return std::string 
         */
        std::string createLogFileName();

};