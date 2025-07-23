#ifndef TOOLS_HPP_
#define TOOLS_HPP_
#include <atomic>
#include <cstdint>
#include <iostream>
template <typename Func, typename Object, typename... Args>
  requires std::is_void_v<
      std::invoke_result_t<Func, Object *, Args...>> // 限定返回类型为 void
void SafeCall(Object *obj, Func &&func, Args &&...args) {
  if (obj) {                                   // 检查对象指针是否为空
    (obj->*func)(std::forward<Args>(args)...); // 调用成员函数
  } else {
    std::cerr << "Object pointer is null, function call skipped." << std::endl;
  }
}

class IdAllocator {
  std::atomic_uint32_t _current_id = 0;

public:
  uint32_t AllocateId() {
    return _current_id.fetch_add(1, std::memory_order_relaxed);
  }
  void Reset() { _current_id.store(0, std::memory_order_relaxed); }
};

class NoCopyable {
public:
  NoCopyable() = default;
  NoCopyable(const NoCopyable &) = delete;
  NoCopyable &operator=(const NoCopyable &) = delete;
  NoCopyable(NoCopyable &&) = default;
  NoCopyable &operator=(NoCopyable &&) = default;
  virtual ~NoCopyable() = default;
};


#endif // TOOLS_HPP_
