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
#include "utils/assertions.hpp"

namespace lfds {

    /**
     * @brief A lock free, low overhead, single producer/consumer queue.
     * No locks of any kind are used, so no locking/context switch overhead.
     * This is achieved using atomic variables.
     * @note Storage for queue is dynamically allocated at construction
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
        
        SPSCQueue(const SPSCQueue&) = delete;
        SPSCQueue& operator=(const SPSCQueue&) = delete;

        SPSCQueue(SPSCQueue&&) = delete;
        SPSCQueue& operator=(SPSCQueue&&) = delete;

        /**
         * @brief Get the number of elements in the queue. 
         * Not just how many were allocated, how many have actually been written.
         * @return size_t 
         */
        size_t size() const noexcept {
            return size_;
        }


        /**
         * @brief Get a pointer to the next element to write to. Object is already either default constructed or has prior value from an earlier call.
         * Needs to be implemented this way (two methods) due to atomics.
         * @note Remember to call updateWriteIndex() after writing! 
         * @return T* - pointer to element.
         */
        T* getNextWriteTo() noexcept {
            return &data_[next_write_index_];
        }
        
        /**
         * @brief Increment write index, wrapping around if required. Also increments size of queue.
         */
        void updateWriteIndex() noexcept {
            next_write_index_ = (next_write_index_ + 1) % data_.size();
            ++size_;
        }

        /**
         * @brief Get a pointer to the next object to be read. 
         * Needs to be implemented this way (two methods) due to atomics.
         * @note Remember to call updateReadIndex() after reading!
         * @return T* 
         */
        T* getNextRead() noexcept {
            return (size_ == 0) ? 
                nullptr : &data_[next_read_index_];
        }

        /**
         * @brief Increment the read index, decrement size of the queue.
         */
        void updateReadIndex() noexcept {
            next_read_index_ = (next_read_index_ + 1) % data_.size();
            utils::ASSERT(size_ > 0, "Attempted to read from empty queue!");
            --size_;
        }        
        
    };
}