add_executable(
    LoggerTests
    logger_test.cpp
)

target_link_libraries(
    LoggerTests 

    Logger

    GTest::gtest_main 
    GTest::gtest 
    GTest::gmock 
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(LoggerTests)