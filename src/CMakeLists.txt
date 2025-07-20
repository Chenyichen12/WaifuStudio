

set(waifu_source main.cpp)


# sources
file(GLOB waifu_render_core_source ./render_core/*.cpp ./render_core/*.h)


#
find_package(Vulkan REQUIRED)

add_executable(
    waifu_studio 
    ${waifu_source}
    ${waifu_render_core_source}
)

target_link_libraries(waifu_studio PUBLIC glfw imgui Vulkan::Vulkan)
target_include_directories(waifu_studio PUBLIC ${Vulkan_INCLUDE_DIR})