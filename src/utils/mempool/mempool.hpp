#pragma once
/**
 * @file mempool.h
 * @brief Dynamically allocated memory pool class
 * @version 0.1
 * @date 2023-08-13
 * @test tests/utils/mempool_test.cpp
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstdint>
#include <vector>
#include <string>
#include "assertions.hpp"

namespace utils {

    /**
     * @brief Dynamically allocated memory pool. A large amount of memory is allocated up front, during construction.
     * Then, you can request chunks of memory from the pool, and this will be much faster as the memory has already been allocated.
     * @tparam T Type of object held in pool, must have a default constructor
     */
    template<typename T>
    class MemPool final {

    private:
        struct Block {
            T data;
            bool is_free { true };
        };

        std::vector<Block> m_data;
        size_t m_next_free_index { 0 };

    public:
        /**
         * @brief Create a new memory pool. Pool is initialized with all elements default constructed and marked as free.
         * @param size Number of elements (of type T) in pool. 
         * @note T must have a default constructor
         */
        explicit MemPool(int size)
            : m_data(size, { T(), true }) 
        {
            // This assert is required to ensure safety of an efficiency trick used when deallocating - see MemPool::deallocate().
            ASSERT(reinterpret_cast<const Block*>(&(m_data[0].data)) == &(m_data[0]), "Alignment error; T is not first member of struct.");
        }
        
        // Delete default ctor, copy ctor/assignment, move ctor/assigment as they don't make sense for a memory pool.
        MemPool() = delete;

        MemPool(const MemPool&) = delete;
        MemPool& operator=(const MemPool&) = delete;

        MemPool(MemPool&&) = delete;
        MemPool& operator=(MemPool&&) = delete;

        /**
         * @brief Resize pool to a new size.
         * @note If resizing to a smaller size, elements that don't fit will be destroyed
         * @param new_size New size of pool. Can be larger or smaller than current size.
         */
        void resize(size_t new_size) {
            m_data.resize(new_size, { T(), true });
        }

        /**
         * @brief Allocate a new object in the next free block of the memory pool.
         * 
         * @tparam Args types of arguments
         * @param args arguments forwarded to ctor of object being allocated
         * @return Pointer to new object
         */
        template<typename... Args>
        T* allocate(Args... args) noexcept {
            Block* ptr { &(m_data[m_next_free_index]) };
            ASSERT(ptr->is_free, "Pool is full.");
        
            T* obj { &(ptr->data) };

            // Construct new T, forwarding arguments, directly inside obj
            new(obj) T(std::forward<Args>(args)...);
            ptr->is_free = false;

            updateFreeIndex();
            return obj;
        }

        /**
         * @brief Deallocate the element inside the pool given by the ptr elem.
         * Checks that pointer points to a valid element inside the pool. If it points to something not in the pool, or to something that's already been deallocated,
         * an assertion error will 
         * @param elem Pointer to the element to be deallocated
         */
        void deallocate(const T* elem) noexcept {
            auto index { reinterpret_cast<const Block*>(elem) - &m_data[0] };
            
            // If the given pointer doesn't point to an element within the pool, this index will be out of bounds
            ASSERT(index >= 0 && index < m_data.size(), "Pointer provided doesn't point to something in this pool.");
            
            ASSERT(!m_data[index].is_free, "Attempted to deallocate unallocated entry in pool.");
            m_data[index].is_free = true;
        };

    private:
        /**
         * @brief Internal helper method called when pool changes to find
         * and set the next free index.
         * The idea is to start from the initial index and walk forward, trying to find a free block, since that's where we are most likely to find a free block.
         * If we don't find one, we start from 0 (maybe something was deallocated) and walk up to initial_free_index.
         * If we still don't find one, the memory pool is full and we log a fatal error.
         */
        void updateFreeIndex() noexcept { 
            const auto initial_free_index { m_next_free_index };
            while (!m_data[m_next_free_index].is_free) {
                ++m_next_free_index;
                
                // At the highest index of our memory pool and haven't found a free block. Let's check from 0 up to initial_free_index
                if (m_next_free_index == m_data.size()) [[unlikely]] {
                    m_next_free_index = 0;
                }

                // TODO implement better solution for when we're out of memory rather than logging and terminating.
                if (m_next_free_index == initial_free_index) [[unlikely]] {
                    FATAL("Memory pool out of space.");
                }
            }
        }
    };
}
