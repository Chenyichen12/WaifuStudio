project(WaifuStudio)

file(GLOB_RECURSE ModelSource model/*.h model/*.cc)

file(GLOB ViewSource views/*.h views/*.cc)

set(SOURCE ${ModelSource} ${ViewSource} mainwindow.cc psdparser.cc)

file(GLOB HEADER include/*.h)

file(GLOB UI ui/*.ui)

find_package(glm CONFIG REQUIRED)
find_package(CDT CONFIG REQUIRED)

add_library(WaifuStudio STATIC ${SOURCE} ${HEADER} ${UI})

target_include_directories(WaifuStudio PUBLIC include)
target_include_directories(WaifuStudio PRIVATE ../src)
target_include_directories(WaifuStudio PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_finalize_target(WaifuStudio)

target_link_libraries(WaifuStudio
        Qt6::Core Qt6::Widgets Qt6::Gui
        Qt6::OpenGLWidgets Qt6::OpenGL
        PhotoshopAPI
        glm::glm
        CDT::CDT
)


qt_add_resources(QRC ../res/icon.qrc ../res/shader.qrc)
add_executable(WaifuStudioApp app/main.cc ${QRC})
target_include_directories(WaifuStudioApp PRIVATE include)
target_link_libraries(WaifuStudioApp PRIVATE WaifuStudio)
