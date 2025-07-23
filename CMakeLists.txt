cmake_minimum_required(VERSION 3.20)

project(WaifuStudio LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
add_compile_options("/utf-8" "/EHsc")

endif()

add_subdirectory(vendor)
add_subdirectory(src)