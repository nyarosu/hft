#include <gtest/gtest.h>
#include "networking/tcp_socket.hpp"

TEST(CreateAndConnect, TCPSocketTests) {
    logger::Logger logger { "test" };
    networking::TCPSocket socket { logger };
    int fd { socket.connect("www.google.com", "enp3s0", 80, false) };
    ASSERT_NE(fd, -1);
}

TEST(CreateAndConnectAndSend, TCPSocketTests) {
    logger::Logger logger { "test2" };
    networking::TCPSocket socket { logger };
    int fd { socket.connect("www.google.com", "enp3s0", 80, false) };
    ASSERT_NE(fd, -1);
    char data[] { "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n" };
    int len { socket.send(data, sizeof(data)) };
    bool res { socket.sendAndRecv() };
    ASSERT_TRUE(res);
    ASSERT_EQ(len, sizeof(data));
}