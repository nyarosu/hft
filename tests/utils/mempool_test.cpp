#include "mempool/mempool.hpp"
#include <gtest/gtest.h>

TEST(MemPool, PoolIsInitialized)
{
    utils::MemPool<int> pool{10};
    pool.allocate();
}