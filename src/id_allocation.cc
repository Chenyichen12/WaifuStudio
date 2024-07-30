//
// Created by chen_yichen on 2024/7/30.
//
#include "include/id_allocation.h"
IdAllocation::IdAllocation()= default;
IdAllocation &IdAllocation::getInstance() {
  static auto allocation = IdAllocation();
  return allocation;
}
int IdAllocation::allocate() {
  std::lock_guard<std::mutex> lockGuard(m);
  if(!this->hole.empty()){
    auto which = *(hole.end()-1);
    hole.pop_back();
    return which;
  }

  this->max++;
  return max;

}
void IdAllocation::release(int which) {
  std::lock_guard<std::mutex> lockGuard(m);
  if(which <= 0){
    return;
  }
  if(which == max){
    max--;
    return;
  }

  auto inContainer = std::find(hole.begin(),hole.end(),which);
  if(inContainer != hole.end()){

    return;
  }

  hole.push_back(which);
}
void IdAllocation::releaseAll() {
  std::lock_guard<std::mutex> lockGuard(m);
  this->max = 0;
  this->hole.clear();
}
