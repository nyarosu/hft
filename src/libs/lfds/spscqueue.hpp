#pragma once
/**
 * @file spscqueue.hpp
 * @brief Single producer, single consumer, lock free queue
 * @version 0.1
 * @date 2023-08-14
 * @test tests/lfds/spscqueue_test.cpp
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <vector>
#include <atomic>

namespace lfds {

    /**
     * @brief A simple, low overhead, single producer, single consumer lock free queue.
     * No locks of any kind are used, so no locking/context switch overhead.
     * This is achieved using atomic variables.
     * @tparam T Type of objects contained by the queue
     */
    template<typename T>
    class SPSCQueue final {
    private:
        std::vector<T> data_{};
        
        // Next unread index
        std::atomic<size_t> next_read_index_ {0};

        // Next index to write to
        std::atomic<size_t> next_write_index_ {0};

        std::atomic<size_t> size_ {0};
    public:
        /**
         * @brief Create new SPSC queue, with given size and all elements default constructed
         * @param size Number of elements to dynamically allocate
         */
        SPSCQueue(std::size_t size)
            : data_(size, T())
        {}

        // Delete default, copy, move ctors and assignment operators
        SPSCQueue() = delete;
        
        SPSCQUeue(const SPSCQueue&) = delete;
        SPSCQueue& operator=(const SPSCQueue&) = delete;

        SPSCQueue(SPSCQueue&&) = delete;
        SPSCQueue& operator=(SPSCQueue&&) = delete;
        
    };
}