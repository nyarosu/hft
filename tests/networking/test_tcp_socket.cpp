#include <gtest/gtest.h>
#include "networking/tcp_socket.hpp"
#include "test_utils/tcp_socket_fixture.hpp"
#include <optional>

class TCPSocketTests : public TCPSocketFixture {};

TEST_F(TCPSocketTests, CreateTCPSocket) {
    int fd { socket_->connect("www.google.com", "enp3s0", 80, false) };
    ASSERT_NE(fd, -1);
}

TEST_F(TCPSocketTests, ConnectToServer) {
    int tempSock = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(server_port_);
inet_pton(AF_INET, server_addr_.c_str(), &addr.sin_addr);

int result = connect(tempSock, (struct sockaddr*)&addr, sizeof(addr));
if (result == 0) {
    // Successfully connected, server is listening
    close(tempSock);  // Important: Close the temporary socket after testing
} else {
    ASSERT_TRUE(false);
}
}

TEST_F(TCPSocketTests, SendDataTCPSocket) {
    // Connect to test server
    int fd { socket_->connect(server_addr_, "lo", server_port_, false) };
    ASSERT_NE(fd, -1);

    // Accept connection on server
    auto server_client_socket { acceptClient() };
    ASSERT_NE(server_client_socket, nullptr);
    
    // Send some data
    std::string data { "Hello World!" };
    int ret { socket_->send(data.c_str(), data.size()) };
    ASSERT_EQ(ret, data.size());

    bool recieved { socket_->sendAndRecv() };

    // We aren't expecting to receive any data
    ASSERT_FALSE(recieved);

    // Wait for server to receive data
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Check that it was received
    char buf[1024];
    auto read { readFromServer(*server_client_socket ,buf, data.size()) };

    ASSERT_TRUE(read.has_value());
    ASSERT_EQ(read.value(), data.size());

    // Check that the data is correct
    std::string read_data { buf, read.value() };
    ASSERT_EQ(read_data, data);
}