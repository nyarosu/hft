add_executable(
    LFDSTests
    test_spscqueue.cpp
)

target_link_libraries(
    LFDSTests 

    LFDS

    GTest::gtest_main 
    GTest::gtest 
    GTest::gmock 
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(LFDSTests)