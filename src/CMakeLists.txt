project(WaifuStudio)

set(SOURCE
        mainwindow.cc
        psdparser.cc
         model/layer.cc
         model/id_allocation.cc
         model/tree_manager.cc
         model/layer_model.cc
         model/layer_bitmap.cc
        model/project.cc
)

file(GLOB HEADER include/*.h)

file(GLOB UI ui/*.ui)


add_library(WaifuStudio STATIC ${SOURCE} ${HEADER} ${UI})

target_include_directories(WaifuStudio PRIVATE include)

target_include_directories(WaifuStudio PRIVATE ${CMAKE_BINARY_DIR})

qt_finalize_target(WaifuStudio)

target_link_libraries(WaifuStudio
        Qt6::Core Qt6::Widgets Qt6::Gui
        PhotoshopAPI
)
add_executable(WaifuStudioApp app/main.cc)
target_include_directories(WaifuStudioApp PRIVATE include)
target_link_libraries(WaifuStudioApp PRIVATE WaifuStudio)
