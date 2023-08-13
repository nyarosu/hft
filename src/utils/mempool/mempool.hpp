#pragma once
/**
 * @file mempool.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-13
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
     * @tparam T 
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
         * @brief Create a new memory pool.
         * Pool is initialized with all elements default constructed and marked as free.
         * @param size Number of elements (of type T) in pool. 
         */
        explicit MemPool(size_t size)
            : m_data(size, { T(), true }) 
        {
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
            Block* ptr { &m_data[m_next_free_index] };
            ASSERT(ptr->is_free, "Next free index is in use.");
            
            // Get a pointer to the object portion of the memory block.
            T* obj { &(ptr->data) };

            // Construct new T, forwarding arguments, directly inside obj
            new(obj) T(std::forward<Args>(args)...);

            updateFreeIndex();
            return obj;
        }

        void deallocate(const T* elem) noexcept {};

    private:
        /**
         * @brief Internal helper method called when pool changes to find
         * and set the next free index.
         * The idea is to start from the initial index and walk forward, trying to find a free block.
         * If we don't find one, we start from 0 (maybe something was deallocated) and walk up to initial_free_index.
         * If we still don't find one, the memory pool is full.
         * @return void
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
