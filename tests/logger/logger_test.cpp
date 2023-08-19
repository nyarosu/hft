#include <gtest/gtest.h>

#include "logger/logger.hpp"
#include "logger_fixture.hpp"

using namespace logger;

TEST_F(LoggerTests, LogSimpleString) {
    logger_->log("Hello, world!");
    logger_->flushQueue();
    
    std::ifstream file { getLogFileName() };
    ASSERT_TRUE(file.is_open());


    std::string line;
    std::getline(file, line);
    ASSERT_EQ(line, "Hello, world!");
}

TEST_F(LoggerTests, LogFormatString) {
    
    logger_->log("Hello, %!", "world");
    logger_->flushQueue();
    
    std::ifstream file { getLogFileName() };
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    ASSERT_EQ(line, "Hello, world!");
}

TEST_F(LoggerTests, LogComplexFormatStringMultipleLines) {
    
    logger_->log("Hello, % % % % % % % % % %\n", "world", 2, 5.03, 5.03f, 5, 5l, 5ll, 5u, 5ul, 5ull);
    logger_->log("Another line, line number %, test name %\n", 2, getLogFileName());
    logger_->flushQueue();
    
    std::ifstream file { getLogFileName() };
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    ASSERT_EQ(line, "Hello, world 2 5.03 5.03 5 5 5 5 5 5");

    std::getline(file, line);
    ASSERT_EQ(line, "Another line, line number 2, test name " + getLogFileName());
}


TEST_F(LoggerTests, ReuseLogFile) {
    
    logger_->log("Hello, world! From Logger %\n", 1);
    logger_->flushQueue();
    
    Logger second { getLogFileName() };
    second.log("Hello, world! From Logger %\n", 2);
    second.flushQueue();

    std::ifstream file { getLogFileName() };
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::getline(file, line);
    ASSERT_EQ(line, "Hello, world! From Logger 1");

    std::getline(file, line);
    ASSERT_EQ(line, "Hello, world! From Logger 2");
}
