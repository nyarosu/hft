#include <gtest/gtest.h>

#include "logger/logger.hpp"

TEST(LoggerTests, BasicLogging) {
    logger::Logger logs { "test.log" };
}