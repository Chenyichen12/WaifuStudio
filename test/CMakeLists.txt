project(Test)

enable_testing()

find_package(GTest CONFIG REQUIRED)


set(TestSource  
    main.cc
    psdparser_test.cc
    tree_manager_test.cc
    spriterendergroup_test.cc
    mathtool_test.cc
    cdt_test.cc
)
add_executable(Test ${TestSource})
target_link_libraries(Test PUBLIC GTest::gtest GTest::gtest_main WaifuStudio)

target_include_directories(Test PRIVATE ../src)


qt_add_resources(QRC ../res/icon.qrc ../res/shader.qrc)

add_executable(UiTest
uitest_main.cc
${QRC}
)

target_link_libraries(UiTest PUBLIC WaifuStudio)

target_include_directories(UiTest PUBLIC ../src)
include(GoogleTest)

gtest_discover_tests(Test)

