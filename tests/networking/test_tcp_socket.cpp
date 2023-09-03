#include <gtest/gtest.h>
#include "networking/tcp_socket.hpp"

TEST(CreateAndConnect, TCPSocketTests) {
    logger::Logger logger { "test" };
    networking::TCPSocket socket { logger };
    int fd { socket.connect("www.google.com", "enp3s0", 80, false) };
    ASSERT_NE(fd, -1);
}