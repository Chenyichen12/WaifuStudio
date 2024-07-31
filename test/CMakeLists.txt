project(Test)

enable_testing()

find_package(GTest CONFIG REQUIRED)


set(TestSource  
      main.cc
        psdparser_test.cc
        id_allocation_test.cc
        tree_manager_test.cc
)
add_executable(Test ${TestSource})
target_link_libraries(Test PUBLIC GTest::gtest GTest::gtest_main WaifuStudio)

target_include_directories(Test PUBLIC ../src/include)

include(GoogleTest)

gtest_discover_tests(Test)

