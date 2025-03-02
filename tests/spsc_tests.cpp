#include <cassert>
#include <gtest/gtest.h>
#include <string>
#include "../include/pika_queue.hpp"

TEST(spsc_queue_test, starts_empty) {
  Pika_Q<int> q(4);
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(q.size(), 0);
}

TEST(spsc_queue_test, push_pop_single) {
  Pika_Q<int> q(4);
  EXPECT_TRUE(q.push(2));
  int val;
  EXPECT_TRUE(q.pop(val));
  EXPECT_EQ(val, 2);
}

TEST(spsc_queue_test, push_till_full) {
  Pika_Q<int> q(4);
  
  EXPECT_TRUE(q.push(1));
  EXPECT_TRUE(q.push(2));
  EXPECT_TRUE(q.push(3));
  EXPECT_TRUE(q.push(4));
  
  EXPECT_FALSE(q.push(5));
  EXPECT_TRUE(q.full());
}

TEST(spsc_queue_test, pop_until_empty) {
  Pika_Q<int> q(4);
  q.push(1); q.push(2); q.push(3); q.push(4);

  int val {};
  EXPECT_TRUE(q.pop(val));
  EXPECT_EQ(val, 1);

  EXPECT_TRUE(q.pop(val));
  EXPECT_EQ(val, 2);

  EXPECT_TRUE(q.pop(val));
  EXPECT_EQ(val, 3);
  
  EXPECT_TRUE(q.pop(val));
  EXPECT_EQ(val, 4);

  EXPECT_FALSE(q.pop(val));
  EXPECT_TRUE(q.empty());
}

TEST(spsc_queue_test, test_size) {
  Pika_Q<int> q(4);
  
  q.push(1);
  EXPECT_EQ(q.size(), 1);
  q.push(2);
  EXPECT_EQ(q.size(), 2);
  q.push(3);
  EXPECT_EQ(q.size(), 3);
  q.push(4);
  EXPECT_EQ(q.size(), 4);

  q.push(5);
  EXPECT_EQ(q.size(), 4);
  
  int val {};

  q.pop(val);
  EXPECT_EQ(q.size(), 3);
  
  q.pop(val);
  EXPECT_EQ(q.size(), 2);

  q.pop(val);
  EXPECT_EQ(q.size(), 1);

  q.pop(val);
  EXPECT_EQ(q.size(), 0);

  q.push(1);
  EXPECT_EQ(q.size(), 1);
}

TEST(spsc_queue_test, test_wrap_around) {
  Pika_Q<int> q(4);
  
  q.push(1); q.push(2); q.push(3); q.push(4);

  int val {};
  q.pop(val); q.pop(val);

  q.push(5); q.push(6);

  q.pop(val);
  EXPECT_EQ(val, 3);
  q.pop(val);
  EXPECT_EQ(val, 4);
  q.pop(val);
  EXPECT_EQ(val, 5);
  q.pop(val);
  EXPECT_EQ(val, 6);
}

TEST(spsc_queue_test, test_string_elems) {
  Pika_Q<std::string> q(4);
  
  EXPECT_TRUE(q.push("hello"));
  EXPECT_TRUE(q.push("world"));

  std::string val {};
  q.pop(val);
  EXPECT_EQ(val, "hello");
  
  q.pop(val);
  EXPECT_EQ(val, "world");
}

class Obj {
public:
  Obj(std::string s, int i) : label(s), val(i) {}
  
  std::string label;
  int val;
};

TEST(spsc_queue_test, test_emplace) {
  Pika_Q<Obj> q(4);

  ASSERT_TRUE(q.emplace("hello", 1));
  ASSERT_TRUE(q.emplace("world", 2));
  
  Obj ob("empty", 0);
  
  ASSERT_TRUE(q.pop(ob));
  EXPECT_EQ(ob.label, "hello");
  EXPECT_EQ(ob.val, 1);
  
  ASSERT_TRUE(q.pop(ob));
  EXPECT_EQ(ob.label, "world");
  EXPECT_EQ(ob.val, 2);


  ASSERT_TRUE(q.empty());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
