#include <gtest/gtest.h>
#include "engine/memtable.hpp"

TEST(MemTableTest, PutAndGetBasic) {
MemTable memtable;

memtable.Put("user:1", "alice");
memtable.Put("user:2", "bob");

auto res1 = memtable.Get("user:1");
EXPECT_TRUE(res1.found);
EXPECT_FALSE(res1.is_deleted);
EXPECT_EQ(res1.value, "alice");

auto res2 = memtable.Get("user:2");
EXPECT_TRUE(res2.found);
EXPECT_FALSE(res2.is_deleted);
EXPECT_EQ(res2.value, "bob");

auto res3 = memtable.Get("user:unknown");
EXPECT_FALSE(res3.found);
}

TEST(MemTableTest, SequentialVersioningAndTombstone) {
MemTable memtable;

// 1. First insert
memtable.Put("key1", "val1");
auto res1 = memtable.Get("key1");
EXPECT_TRUE(res1.found);
EXPECT_FALSE(res1.is_deleted);
EXPECT_EQ(res1.value, "val1");

// 2. Rewriting with new value
memtable.Put("key1", "val2");
auto res2 = memtable.Get("key1");
EXPECT_TRUE(res2.found);
EXPECT_FALSE(res2.is_deleted);
EXPECT_EQ(res2.value, "val2");

// 3. Deleting with higher seqnum
memtable.Delete("key1");
auto res3 = memtable.Get("key1");
EXPECT_TRUE(res3.found);
EXPECT_TRUE(res3.is_deleted);
EXPECT_TRUE(res3.value.empty());
}

TEST(MemTableTest, DeleteIncreasesMemoryUsage) {
MemTable memtable;

memtable.Put("key1", "value1");
size_t mem_before_del = memtable.ApproximateMemoryUsage();

// Deleting allocate new node
memtable.Delete("key1");
size_t mem_after_del = memtable.ApproximateMemoryUsage();

EXPECT_GT(mem_after_del, mem_before_del);
}