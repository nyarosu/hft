#include <gtest/gtest.h>
#include "assertions.hpp"

TEST(AssertionsTests, AssertingFalseTerminates)
{
    ASSERT_DEATH(utils::ASSERT(false, "test"), "Assertion failed: test");
}

TEST(AssertionsTests, AssertingTrueIsOK)
{
    ASSERT_NO_FATAL_FAILURE(utils::ASSERT(true, "test"));
}

TEST(AssertionsTests, FatalQuitsImmediately)
{
    ASSERT_DEATH(utils::FATAL("test"), "Fatal error: test");
}