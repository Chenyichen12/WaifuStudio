cmake_minimum_required(VERSION 3.27)
project(WaifuStudio)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Widgets
        Gui
        OpenGLWidgets
        OpenGL
        REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/permissive-")
    add_compile_options("/Zc:__cplusplus")
endif ()

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(test)
