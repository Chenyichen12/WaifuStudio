project(WaifuStudio)

set(SOURCE
        mainwindow.cc
        psdparser.cc
        layer.cc
        id_allocation.cc
        tree_manager.cc
)

file(GLOB HEADER include/*.h)

file(GLOB UI ui/*.ui)

add_library(WaifuStudio STATIC ${SOURCE} ${HEADER} ${UI})

qt_finalize_target(WaifuStudio)

target_link_libraries(WaifuStudio
        Qt6::Core Qt6::Widgets Qt6::Gui
        PhotoshopAPI
)
add_executable(WaifuStudioApp app/main.cc)
target_include_directories(WaifuStudioApp PRIVATE include)
target_link_libraries(WaifuStudioApp PRIVATE WaifuStudio)
