
add_subdirectory(glfw)

find_package(Vulkan REQUIRED)
set(imgui_source 
   imgui/imgui_draw.cpp
   imgui/imgui_demo.cpp
   imgui/imgui_tables.cpp
   imgui/imgui_widgets.cpp
   imgui/backends/imgui_impl_glfw.cpp
   imgui/backends/imgui_impl_vulkan.cpp
)
add_library(imgui ${imgui_source})
target_link_libraries(imgui PRIVATE Vulkan::Vulkan glfw)
target_include_directories(imgui PUBLIC ./imgui PRIVATE glfw/include ${Vulkan_INCLUDE_DIR})

# single head
add_library(single_head single_head.cpp)
target_include_directories(single_head 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb)
target_link_libraries(single_head PRIVATE Vulkan::Vulkan)