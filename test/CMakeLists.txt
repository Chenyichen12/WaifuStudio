project(Test)

enable_testing()

find_package(GTest CONFIG REQUIRED)
add_executable(Test
        main.cc
        psdparser_test.cc
        id_allocation_test.cc)
target_link_libraries(Test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main WaifuStudio)

target_include_directories(Test PRIVATE ../src/include)

#add_test(AllTestsInMain Test)
