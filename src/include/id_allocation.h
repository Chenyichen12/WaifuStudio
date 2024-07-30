//
// Created by chen_yichen on 2024/7/30.
//

#pragma once
#include "vector"
#include "mutex"
class IdAllocation{
private:
  IdAllocation();
  static IdAllocation& instance;
  std::vector<int>hole;
  int max;

  std::mutex m;
public:
  IdAllocation(const IdAllocation&) = delete;
  static IdAllocation& getInstance();
  const IdAllocation &operator=(const IdAllocation &single) = delete;

  int allocate();
  void release(int which);

  void releaseAll();
};
