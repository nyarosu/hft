add_executable(
    LoggerTests
    logger_test.cpp
)

target_link_libraries(
    LoggerTests 

    Logger

    TestUtils

    GTest::gtest_main 
    GTest::gtest 
    GTest::gmock 
    GTest::gmock_main
)

target_include_directories(LoggerTests PUBLIC .)

include(GoogleTest)
gtest_discover_tests(LoggerTests)