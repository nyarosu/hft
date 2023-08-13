add_executable(
    UtilsTests
    threads_test.cpp
    assertions_test.cpp
    mempool_test.cpp
)

target_link_libraries(
    UtilsTests 

    Utils

    GTest::gtest_main 
    GTest::gtest 
    GTest::gmock 
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(UtilsTests)