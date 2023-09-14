#include <gtest/gtest.h>
#include "networking/tcp_socket.hpp"
#include "test_utils/tcp_socket_fixture.hpp"
#include <optional>

class TCPSocketTests : public TCPSocketFixture {};

TEST_F(TCPSocketTests, CreateTCPSocket) {
    int fd { socket_->connect("www.google.com", "enp3s0", 80, false) };
    ASSERT_NE(fd, -1);
}

// This doesn't quite test the TCP socket class, it just tests our test server
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

TEST_F(TCPSocketTests, SendToServer) {
    auto server_client_socket { connectToServer() };
    
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
    std::string read_data { buf, static_cast<long unsigned int>(read.value()) };
    ASSERT_EQ(read_data, data);
}

TEST_F(TCPSocketTests, ReadFromServer) {
    auto server_client_socket { connectToServer() };

    server_client_socket->send("Hello World!", 12);
    server_client_socket->sendAndRecv();

    // Read from client
    char buf[1024];
    bool received { socket_->sendAndRecv() };
    ASSERT_TRUE(received);

    // Check that the data is correct
    std::string read_data { socket_->recv_buf_.get() , 12 };
    ASSERT_EQ(read_data, "Hello World!");
}

TEST_F(TCPSocketTests, InvalidConnectionRejected) {
    int fd { socket_->connect("invalid.abcdhaoushdaous.com", "", 80, false) };
    ASSERT_EQ(fd, -1);
}

TEST_F(TCPSocketTests, MultipleSends) {
    auto server_client_socket { connectToServer() };
    
    // Send some data
    std::string data { generateRandomPayload(200) };

    // Send data in chunks
    int ret { socket_->send(data.c_str(), 100) };
    ASSERT_EQ(ret, 100);

    ret = socket_->send(data.c_str() + 100, 100);
    ASSERT_EQ(ret, 100);

    bool recieved { socket_->sendAndRecv() };
    // We aren't expecting to receive any data
    ASSERT_FALSE(recieved);

    // Check that it was received
    char buf[1024];
    auto read { readFromServerBlocking(*server_client_socket ,buf, data.size()) };

    ASSERT_TRUE(read.has_value());
    ASSERT_EQ(read.value(), data.size());

    // Check that the data is correct
    std::string read_data { buf, static_cast<long unsigned int>(read.value()) };
    ASSERT_EQ(read_data, data);
}