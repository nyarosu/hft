#include "spscqueue.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace lfds;

// Basic Creation and Write/Read
TEST(SPSCQueueTests, CreateSPSCQueue) {
    SPSCQueue<std::string> queue { 5 };

    // Write two elements - abc and def. 
    auto write_ptr { queue.getNextWriteTo() };
    *write_ptr = "abc";
    queue.updateWriteIndex();

    auto write_ptr2 { queue.getNextWriteTo() };
    *write_ptr2 = "def";
    queue.updateWriteIndex();

    auto read_ptr { queue.getNextRead() };
    ASSERT_STREQ(read_ptr->c_str(), "abc");
    queue.updateReadIndex();
}

// Read from Empty Queue
TEST(SPSCQueueTests, ReadFromEmptyQueue) {
    SPSCQueue<std::string> queue { 5 };
    ASSERT_EQ(queue.getNextRead(), nullptr);
}

// Wrap-around Behavior
TEST(SPSCQueueTests, WrapAroundBehavior) {
    SPSCQueue<int> queue { 3 };

    for(int i = 0; i < 3; ++i) {
        auto write_ptr { queue.getNextWriteTo() };
        *write_ptr = i;
        queue.updateWriteIndex();
    }

    for(int i = 0; i < 2; ++i) {
        queue.updateReadIndex();  // Consume two elements
    }

    auto write_ptr { queue.getNextWriteTo() };
    *write_ptr = 3;
    queue.updateWriteIndex();

    auto read_ptr { queue.getNextRead() };
    ASSERT_EQ(*read_ptr, 2);  // Expecting the 3rd element, which is 2
}

// Ordering of Multiple Writes/Reads
TEST(SPSCQueueTests, OrderingOfElements) {
    SPSCQueue<std::string> queue { 4 };
    std::vector<std::string> test_strings = {"one", "two", "three", "four"};
    
    for(const auto& str : test_strings) {
        auto write_ptr { queue.getNextWriteTo() };
        queue.updateWriteIndex();
        *write_ptr = str;
    }

    for(const auto& expected_str : test_strings) {
        auto read_ptr { queue.getNextRead() };
        ASSERT_STREQ(read_ptr->c_str(), expected_str.c_str());
        queue.updateReadIndex();
    }
}

TEST(SPSCQueueTests, BasicMultithreadedProducerConsumer) {
    const int NUM_ITEMS { 50 };
    SPSCQueue<int> queue { NUM_ITEMS * 2 };  // create a queue of a sufficient size

    // Producer function
    auto producerFunc = [&queue]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            auto write_ptr = queue.getNextWriteTo();
            *write_ptr = i;
            queue.updateWriteIndex();

            std::this_thread::sleep_for(std::chrono::microseconds(1));  // Simulate some work
        }
    };

    // Consumer function
    auto consumerFunc = [&queue]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            while (true) {
                auto read_ptr = queue.getNextRead();
                if (read_ptr) {
                    ASSERT_EQ(*read_ptr, i);
                    queue.updateReadIndex();
                    break;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(1));  // Wait and try again
            }
        }
    };

    std::thread producerThread(producerFunc);
    std::thread consumerThread(consumerFunc);

    producerThread.join();
    consumerThread.join();
}

TEST(SPSCQueueTests, AdvancedMultithreadedProducerConsumer) {
    
}
