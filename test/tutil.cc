#include "tell/util.h"
#include <gtest/gtest.h>

#include <iostream>
#include <cmath>
#include <cassert>

#include <unistd.h>

double square_root(double x)
{
  assert(x >= 0);
  return std::sqrt(x);
}

TEST(SquareRootTest, Zero)
{
  ASSERT_EQ(0.0, square_root(0.0));
}

TEST(SquareRootTest, Positive)
{
  ASSERT_EQ(16.0, square_root(256));
}

TEST(SquareRootTest, Negative)
{
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  // can't catch core dump
  // ASSERT_EXIT(square_root(-1.0), ::testing::ExitedWithCode(134),
  // 	      "Assertion `x >= 0' failed.");
  ASSERT_DEATH(square_root(-1.0), "Assertion `x >= 0' failed.");
}

TEST(StopWatchTest, Basics)
{
  tell::Stop_watch w("test");
  sleep(1);
  
}

int main(int argc, char* argv[])
{
  ::tell::Stop_watch w;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
