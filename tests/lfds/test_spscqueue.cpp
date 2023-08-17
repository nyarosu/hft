#include "spscqueue.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace lfds;

// Simple creation, write/read
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

// Reading from an empty queue should fail (return nullptr)
TEST(SPSCQueueTests, ReadFromEmptyQueue) {
    SPSCQueue<std::string> queue { 5 };
    ASSERT_EQ(queue.getNextRead(), nullptr);
}

// Wrap around to writing at 0 when queue is full
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
    queue.updateReadIndex();

    auto value_at_index_0{ queue.getNextRead() };
    queue.updateReadIndex();
    ASSERT_EQ(*value_at_index_0, 3); // Expecting the 1st element, which is 3
}

// Queue should read/write in the correct order
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

// Simple multithreaded scenario
TEST(SPSCQueueTests, BasicMultithreadedProducerConsumer) {
    const int NUM_ITEMS { 50 };
    SPSCQueue<int> queue { NUM_ITEMS }; 

    // Producer function
    auto producerFunc = [&queue]() {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            auto write_ptr = queue.getNextWriteTo();
            *write_ptr = i;
            queue.updateWriteIndex();

            std::this_thread::sleep_for(std::chrono::microseconds(1));
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
                // Too early to read, try again after waiting
                std::this_thread::sleep_for(std::chrono::microseconds(1)); 
            }
        }
    };

    std::thread producerThread(producerFunc);
    std::thread consumerThread(consumerFunc);

    producerThread.join();
    consumerThread.join();
}
