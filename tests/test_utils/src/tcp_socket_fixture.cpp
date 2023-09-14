/**
 * @file tcp_socket_fixture.cpp
 * @brief TCP Socket testing fixture
 * @copyright Copyright (c) 2023
 * 
 */
#include "test_utils/tcp_socket_fixture.hpp"
#include "networking/tcp_socket.hpp"
#include <optional>

void TCPSocketFixture::SetUp() {
    LoggingFixture::SetUp();
    socket_ = std::make_unique<networking::TCPSocket>(*logger_);

    // Create server
    server_ = std::make_unique<networking::TCPSocket>(*logger_);
    int serverFd { server_->connect("127.0.0.1", "lo", 0, true) };
    ASSERT_NE(serverFd, -1);

    // We need to retrieve the port number of the server socket, since we used 0 to auto select an available port
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(0);

    socklen_t len = sizeof(addr);
    int ret = getsockname(serverFd, (struct sockaddr *)&addr, &len);
    ASSERT_NE(ret, -1);
    server_sockaddr_ = addr;

    server_port_ = ntohs(addr.sin_port);
    server_addr_ = "127.0.0.1";
}

void TCPSocketFixture::TearDown() {
    LoggingFixture::TearDown();
}

std::unique_ptr<networking::TCPSocket> TCPSocketFixture::acceptClient() {
    std::string time_str_;
    constexpr int MAX_ATTEMPTS { 15 };
    constexpr int SLEEP_TIME_MS { 200 };

    // Accept connection on server, with timeout
    int attempts { 0 };
    socklen_t len { sizeof(server_sockaddr_) };
    while (attempts < MAX_ATTEMPTS) {
        int ret { accept(server_->fd_, (struct sockaddr*)&server_sockaddr_, &len) };
        if (ret == -1) {
            attempts++;
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
            continue;
        }
        // Create a new socket object for the client
        std::unique_ptr<networking::TCPSocket> client_socket { std::make_unique<networking::TCPSocket>(*logger_) };
        client_socket->fd_ = ret;
        return client_socket;
    }
    logger_->log("%:% %() % TCPSocketFixture::acceptClient() failed to accept client\n",
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str_));
    return nullptr;
}

std::optional<int> TCPSocketFixture::readFromServer(networking::TCPSocket& server_client_socket, char* buf, size_t len) {
    // Read from server
    bool recieved { server_client_socket.sendAndRecv() };
    if (!recieved) {
        logger_->log("%:% %() % TCPSocketFixture::readFromServer() failed to receive data from server\n",
        __FILE__, __LINE__, __FUNCTION__);
        return std::nullopt;
    }

    // Write from socket buffer to buf provided in argument, first validating len
    if (len > networking::BUFFER_SIZE || len > server_client_socket.next_rcv_valid_index_) {
        logger_->log("%:% %() % TCPSocketFixture::readFromServer() invalid len:%\n",
        __FILE__, __LINE__, __FUNCTION__, len);
        return std::nullopt;
    }
    memcpy(buf, server_client_socket.recv_buf_.get(), len);

    return len;
}

std::unique_ptr<networking::TCPSocket> TCPSocketFixture::connectToServer() {
    // Connect to test server
    int fd { socket_->connect(server_addr_, "lo", server_port_, false) };

    // Accept connection on server
    auto server_client_socket { acceptClient() };

    return server_client_socket;
};

// Function to generate random data
std::string TCPSocketFixture::generateRandomPayload(size_t length) {
    std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.resize(length);

    for (size_t i = 0; i < length; i++) {
        result[i] = charset[rand() % charset.length()];
    }
    return result;
}

std::optional<int>  TCPSocketFixture::readFromServerBlocking(networking::TCPSocket& server_client_socket, char* buf, size_t len) {
    constexpr int MAX_ATTEMPTS { 15 };
    constexpr int SLEEP_TIME_MS { 200 };
    
    int attempts { 0 };
    while (attempts < MAX_ATTEMPTS) {
        if (server_client_socket.sendAndRecv()) {
            // Write from socket buffer to buf provided in argument, first validating len
            if (len > networking::BUFFER_SIZE || len > server_client_socket.next_rcv_valid_index_) {
                logger_->log("%:% %() % TCPSocketFixture::readFromServer() invalid len:%\n",
                __FILE__, __LINE__, __FUNCTION__, len);
                return std::nullopt;
            }
            memcpy(buf, server_client_socket.recv_buf_.get(), len);

            return len;
        }
        attempts++;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
    }
    logger_->log("%:% %() % TCPSocketFixture::blockUntilServerRecv() failed to receive data from server\n",
        __FILE__, __LINE__, __FUNCTION__);

    return std::nullopt;
}