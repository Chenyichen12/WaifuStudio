project(Test)

enable_testing()

find_package(GTest CONFIG REQUIRED)
add_executable(Test)
target_link_libraries(Test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(AllTestsInMain Test)
