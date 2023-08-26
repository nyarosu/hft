#include "networking/sockets.hpp"
#include "test_utils/logging_fixture.hpp"
#include <gtest/gtest.h>

class SocketUtilsTests : public LoggingFixture {};

TEST_F(SocketUtilsTests, TestGetInterfaceAddress) {
    auto addr { networking::getInterfaceIP("lo") };
    ASSERT_STREQ(addr.c_str(), "127.0.0.1");
}

// Create a simple socket that listens on a local port
TEST_F(SocketUtilsTests, TestListenValidSocket) {
    auto fd { networking::createSocket(*logger_, "127.0.0.1", "lo", 0, false, false, true, 0, false) };
    ASSERT_NE(fd, -1);
    close(fd);
}

TEST_F(SocketUtilsTests, TestInvalidSocketCreation) {
    auto fd { networking::createSocket(*logger_, "google.co.uk", "", 0, false, false, true, 0, false) };
    ASSERT_EQ(fd, -1);
}

TEST_F(SocketUtilsTests, TestConnectValidSocket) {
    auto fd { networking::createSocket(*logger_, "google.co.uk", "", 80, false, false, false, 0, false) };
    ASSERT_NE(fd, -1);
    close(fd);
}