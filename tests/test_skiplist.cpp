#include <gtest/gtest.h>
#include "engine/skiplist.hpp"
#include <string>
#include <vector>

// 1. Put & Get test
TEST(SkipListTest, PutAndGetBasic) {
SkipList<std::string, std::string> list;

list.Put("apple", "red");
list.Put("banana", "yellow");

auto val1 = list.Get("apple");
ASSERT_TRUE(val1.has_value());
EXPECT_EQ(*val1, "red");

auto val2 = list.Get("banana");
ASSERT_TRUE(val2.has_value());
EXPECT_EQ(*val2, "yellow");

EXPECT_FALSE(list.Get("orange").has_value());
}

// 2. Update an existing key
TEST(SkipListTest, OverwriteExistingKey) {
SkipList<std::string, std::string> list;

list.Put("key1", "val1");
EXPECT_EQ(list.Get("key1").value(), "val1");

list.Put("key1", "val2");
EXPECT_EQ(list.Get("key1").value(), "val2");
}

// 3. Stress-input
TEST(SkipListTest, InsertManySequential) {
SkipList<int, int> list;
constexpr int kCount = 1000;

for (int i = 0; i < kCount; ++i) {
list.Put(i, i * 10);
}

for (int i = 0; i < kCount; ++i) {
auto val = list.Get(i);
ASSERT_TRUE(val.has_value());
EXPECT_EQ(*val, i * 10);
}

EXPECT_FALSE(list.Get(kCount).has_value());
}

// 4.  Iterator iterates through the elements in ascending order
TEST(SkipListTest, IteratorTraversesInOrder) {
SkipList<int, std::string> list;

list.Put(50, "fifty");
list.Put(10, "ten");
list.Put(30, "thirty");
list.Put(20, "twenty");
list.Put(40, "forty");

std::vector<int> expected_keys = {10, 20, 30, 40, 50};
std::vector<int> actual_keys;

for (auto it = list.begin(); it != list.end(); ++it) {
actual_keys.push_back(it.key());
}

EXPECT_EQ(actual_keys, expected_keys);
}

// 5. Iterator on an empty list
TEST(SkipListTest, IteratorEmptyList) {
SkipList<std::string, std::string> list;

auto it = list.begin();
EXPECT_EQ(it, list.end());
EXPECT_FALSE(it.Valid());
}
// 6. Find greater or equal
TEST(SkipListTest, FindGreaterOrEqual) {
SkipList<int, std::string> list;
list.Put(10, "ten");
list.Put(20, "twenty");
list.Put(30, "thirty");
list.Put(50, "fifty");

// 1. Exact match
auto it1 = list.FindGreaterOrEqual(20);
ASSERT_TRUE(it1.Valid());
EXPECT_EQ(it1.key(), 20);
EXPECT_EQ(it1.value(), "twenty");

// 2. no key -> take the first larger one (find 25 -> ret 30)
auto it2 = list.FindGreaterOrEqual(25);
ASSERT_TRUE(it2.Valid());
EXPECT_EQ(it2.key(), 30);
EXPECT_EQ(it2.value(), "thirty");

// 3. find key less than min key (find 5 -> ret 10)
auto it3 = list.FindGreaterOrEqual(5);
ASSERT_TRUE(it3.Valid());
EXPECT_EQ(it3.key(), 10);

// 4. find key grater than max key (find 60 -> ret .end())
auto it4 = list.FindGreaterOrEqual(60);
EXPECT_FALSE(it4.Valid());
EXPECT_EQ(it4, list.end());
}