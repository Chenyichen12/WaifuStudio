//
// Created by chen_yichen on 2024/7/30.
//
#include "gtest/gtest.h"
#include "id_allocation.h"
TEST(IdAllocation, TestAllocation){
  auto&& s = IdAllocation::getInstance();
  for (int i = 0; i < 100; ++i) {
    ASSERT_EQ(s.allocate(), i+1);
  }
  s.releaseAll();
  s.allocate();
  s.allocate();
  s.allocate();
  s.release(1);
  s.release(3);
  ASSERT_EQ(s.allocate(),1);
}
