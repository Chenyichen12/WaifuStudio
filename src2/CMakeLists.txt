project(WaifuStudio2)

file(GLOB_RECURSE ModelSource model/*.h model/*.cc model/*.hpp)

file(GLOB_RECURSE ControllerSource controller/*.h controller/*.cc)

file(GLOB ViewSource views/*.h views/*.cc)

set(SOURCE ${ModelSource} ${ViewSource} ${ControllerSource})

file(GLOB HEADER include/*.h)

file(GLOB UI ui/*.ui)

find_package(glm CONFIG REQUIRED)
find_package(CDT CONFIG REQUIRED)

add_library(WaifuStudio2 STATIC ${SOURCE} ${HEADER} ${UI})

target_include_directories(WaifuStudio2 PUBLIC include)
target_include_directories(WaifuStudio2 PRIVATE ../src2)
target_include_directories(WaifuStudio2 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_finalize_target(WaifuStudio2)

target_link_libraries(WaifuStudio2
        Qt6::Core Qt6::Widgets Qt6::Gui
        Qt6::OpenGLWidgets Qt6::OpenGL
        PhotoshopAPI
        glm::glm
        CDT::CDT
)