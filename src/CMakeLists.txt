project(WaifuStudio)

file(GLOB ModelSource model/*.h model/*.cc)

file(GLOB ViewSource views/*.h views/*.cc)

set(SOURCE ${ModelSource} ${ViewSource} mainwindow.cc psdparser.cc)

file(GLOB HEADER include/*.h)

file(GLOB UI ui/*.ui)

add_library(WaifuStudio STATIC ${SOURCE} ${HEADER} ${UI})

target_include_directories(WaifuStudio PUBLIC include)
target_include_directories(WaifuStudio PRIVATE ../src)
target_include_directories(WaifuStudio PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_finalize_target(WaifuStudio)

target_link_libraries(WaifuStudio
        Qt6::Core Qt6::Widgets Qt6::Gui
        PhotoshopAPI
)


qt_add_resources(QRC ../res/icon.qrc)
add_executable(WaifuStudioApp app/main.cc ${QRC})
target_include_directories(WaifuStudioApp PRIVATE include)
target_link_libraries(WaifuStudioApp PRIVATE WaifuStudio)
