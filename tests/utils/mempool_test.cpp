#include "utils/mempool/mempool.hpp"
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

TEST(MemPoolTests, ResizePool) {
    MemPool pool { MemPool<TestObj>{ 2 } };
    pool.allocate(20);
    pool.allocate(50);

    pool.resize(4);

    pool.allocate(20);
    ASSERT_NO_FATAL_FAILURE(pool.allocate(50));
}

TEST(MemPoolTests, ValidDeallocation) {
    MemPool pool { MemPool<TestObj>{ 2 } };
    auto ptr { pool.allocate(15) };

    // No error here means that deallocating succeeded
    ASSERT_NO_THROW(pool.deallocate(ptr));
}

TEST(MemPoolTests, InvalidDeallocation) {
    MemPool pool { MemPool<TestObj>{ 5 } };

    // This object was not allocated inside our pool, so deleting it using pool is illegal
    TestObj* NotInPool { new TestObj{ 5 } };

    ASSERT_DEATH(pool.deallocate(NotInPool), "Pointer provided doesn't point to something in this pool.");

    // Make sure double deallocation is prevented
    TestObj* InPool { pool.allocate(1) };
    pool.deallocate(InPool);

    ASSERT_DEATH(pool.deallocate(InPool), "Attempted to deallocate unallocated entry in pool.");
}

TEST(MemPoolTests, ObjectReuseAfterDeallocation) {
    MemPool pool { MemPool<TestObj>{ 5 } };
    
    TestObj* obj1 { pool.allocate(5) };
    pool.deallocate(obj1);
    
    TestObj* obj2 { pool.allocate(10) };
    
    // obj2 should reuse the memory location of obj1
    ASSERT_EQ(obj1, obj2);
    ASSERT_EQ(obj2->getValue(), 10);
}

TEST(MemPoolTests, PoolExhaustionAndReallocation) {
    MemPool pool { MemPool<TestObj>{ 2 } };
    
    pool.allocate(10);
    pool.allocate(20);
    ASSERT_DEATH(pool.allocate(30), "Memory pool out of space.");

    pool.resize(3);
    TestObj* obj { pool.allocate(30) };
    ASSERT_EQ(obj->getValue(), 30);
}

TEST(MemPoolTests, ResizedPoolIntegrity) {
    MemPool pool { MemPool<TestObj>{ 5 } };

    pool.allocate(10);
    pool.allocate(20);
    pool.allocate(30);
    pool.resize(3);  // Shrinks the pool

    ASSERT_DEATH(pool.allocate(40), "Memory pool out of space.");  // We've filled all slots

    pool.resize(10);  // Expands the pool
    TestObj* obj { pool.allocate(50) };  // Should succeed
    ASSERT_EQ(obj->getValue(), 50);
}



