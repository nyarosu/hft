#include <gtest/gtest.h>
#include "assertions.hpp"

TEST(Assertions, AssertingFalseTerminates)
{
    ASSERT_DEATH(utils::ASSERT(false, "test"), "Assertion failed: test");
}

TEST(Assertions, AssertingTrueIsOK)
{
    ASSERT_NO_FATAL_FAILURE(utils::ASSERT(true, "test"));
}

TEST(Assertions, FatalQuitsImmediately)
{
    ASSERT_DEATH(utils::FATAL("test"), "Fatal error: test");
}