add_subdirectory(src)
add_library(TestUtils ${TEST_UTILS_SOURCES})

target_include_directories(TestUtils PUBLIC ./include)
target_include_directories(TestUtils PRIVATE ./src)

target_link_libraries(TestUtils PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(TestUtils PUBLIC Logger)