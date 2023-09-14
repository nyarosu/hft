add_executable(
    NetworkingTests
    test_socket_utils.cpp
    test_tcp_socket.cpp
    test_tcp_server.cpp
)

target_link_libraries(
    NetworkingTests 

    Networking
    TestUtils

    GTest::gtest_main 
    GTest::gtest 
    GTest::gmock 
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(NetworkingTests)