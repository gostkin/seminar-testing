/*
 * Copyright (c) 2019 Eugene Gostkin
 * Distributed under Apache 2.0 license (see LICENSE FILE)
 */

#include "StdAfxTest.hpp"
#include "Stack.hpp"

TEST(StackActivity, Basic) {
  auto testStack = Stack<int>();
  EXPECT_TRUE(testStack.Empty());
  for (int iteration = 0; iteration != 100; ++iteration)
    testStack.Push(iteration);

  ASSERT_EQ(testStack.Size(), 100);
  ASSERT_FALSE(testStack.Empty());

  for (int iteration = 99; iteration != 49; --iteration) {
    auto res = testStack.Pop();
    EXPECT_EQ(res, iteration);
  }
  EXPECT_EQ(testStack.Size(), 50);
}

TEST(StackSecurity, HeadPointTest) {
  auto * s=new Stack<int>();
  EXPECT_TRUE(s->Empty());
  s->Push(1);
  ASSERT_EQ(s->Size(), 1);
  for (int i = 0; i < 10; ++i)
    s->Push(i);
  int *pointer = reinterpret_cast<int *>(s);
  ++pointer;
  *pointer = 228;
  EXPECT_THROW(s->Pop(), std::runtime_error);
}
