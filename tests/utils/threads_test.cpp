#include <gtest/gtest.h>
#include "threads/threads.hpp"
#include <random>

int getRandCore();

/**
 * @brief Test that threads are successfully created and execute passed function with passed arguments
 */
TEST(ThreadsTests, ThreadsAreCreatedAndRun)
{
    const int CORE_ID{getRandCore()};

    // The test thread will set this int to the sum of its arguments
    int sum_of_args{-1};
    auto test_fn{[&sum_of_args](int num1, int num2, int num3)
                 { sum_of_args = num1 + num2 + num3; }};
    std::thread thread{utils::threads::createAndStart(CORE_ID, "test thread", test_fn, 1, 2, 3)};

    thread.join();
    ASSERT_EQ(sum_of_args, 6);
}

/**
 * @brief Test that threads are set to provided affinity
 */
TEST(ThreadsTests, ThreadsHaveCorrectAffinity)
{
    const int CORE_ID{getRandCore()};

    // The thread will set this variable to its affinity
    int actual_affinity{-1};
    auto test_fn{[&actual_affinity]()
                 {
                     actual_affinity = sched_getcpu();
                 }};

    std::thread thread{utils::threads::createAndStart(CORE_ID, "test thread", test_fn)};

    thread.join();
    ASSERT_EQ(actual_affinity, CORE_ID);
}

/**
 * @brief Get a random hardware core ID
 *
 * @return core ID
 */
int getRandCore()
{
    int n{static_cast<int>(std::thread::hardware_concurrency())};

    std::mt19937 mt{std::random_device{}()};
    std::uniform_int_distribution dist{0, n - 1};

    return dist(mt);
}