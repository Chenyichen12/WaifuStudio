#ifndef TOOLS_HPP_
#define TOOLS_HPP_
#include <iostream>
template<typename Func, typename Object, typename... Args>
requires std::is_void_v<std::invoke_result_t<Func, Object*, Args...>>  // 限定返回类型为 void
void SafeCall(Object* obj, Func&& func, Args&&... args) {
    if (obj) {  // 检查对象指针是否为空
        (obj->*func)(std::forward<Args>(args)...);  // 调用成员函数
    } else {
        std::cerr << "Object pointer is null, function call skipped." << std::endl;
    }
}
#endif // TOOLS_HPP_

