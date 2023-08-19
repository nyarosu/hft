/**
 * @file logger_fixture.cpp
 * @brief Test fixture (common setup/teardown) for logger tests
 * @date 2023-08-18
 * @copyright Copyright (c) 2023
 */

#include "logger_fixture.hpp"
#include <string>
#include <random>

void LoggerTests::SetUp() {
    logFileName_ = createLogFileName();
    logger_ = std::make_unique<logger::Logger>(logFileName_ );
}

void LoggerTests::TearDown() {
    std::remove(logFileName_.c_str());
}

std::string LoggerTests::createLogFileName() {
    // All log files will be named log_<random_number>.log
    std::string logFileName = "log_";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1000000);

    logFileName += std::to_string(dis(gen));
    logFileName += ".log";

    return logFileName;
}