#include "mempool/mempool.hpp"
#include <gtest/gtest.h>

using namespace utils;

class TestObj {
private:
    int m_value {-1};
public:
    TestObj() = default;
    explicit TestObj(int value)
        : m_value { value }
    {}
    int getValue() { return m_value; }
    void setValue(int value) { m_value = value; }
};

TEST(MemPoolTests, PoolSizeValidation) {
    ASSERT_THROW(MemPool<TestObj>{ -10 }, std::exception);
}

TEST(MemPoolTests, CreateAndUseValidPool) {
    // Create empty pool
    MemPool pool { MemPool<TestObj>{ 1000 } };
    
    TestObj* new_obj { pool.allocate(5) };
    
    // Sanity check
    ASSERT_EQ(new_obj->getValue(), 5);
    new_obj->setValue(10);
    ASSERT_EQ(new_obj->getValue(), 10);

    // Allocate another object
    TestObj* new_obj2 { pool.allocate(15) };
    ASSERT_EQ(new_obj2->getValue(), 15);
}

TEST(MemPoolTests, AllocatingFullPoolFails) {
    MemPool pool { MemPool<TestObj>{ 2 } };
    pool.allocate(20);
    pool.allocate(50);
    
    // Pool is full!
    ASSERT_DEATH(pool.allocate(20), "Memory pool out of space.");
}

TEST(MemPoolTests, ResizePool) {}

TEST(MemPoolTests, ValidDeallocation) {}

TEST(MemPoolTests, InvalidDeallocation) {}

