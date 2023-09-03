#pragma once
/**
 * @file tcp_socket_fixture.hpp
 * @brief Test fixture for testing tcp sockets
 * @copyright Copyright (c) 2023
 */

#include <gtest/gtest.h>
#include "networking/tcp_socket.hpp"
#include "logging_fixture.hpp"


class TCPSocketFixture: public LoggingFixture {
protected:
    /**
     * @brief Set up sockets, loggers etc.
     */
    void SetUp() override;

    /**
     * @brief Destroy sockets, delete logger object/file, other cleanup
     */
    void TearDown() override;

    /**
     * @brief Ptr to TCP socket object to be used in tests
     */
    std::unique_ptr<networking::TCPSocket> socket_ { nullptr };

    /**
     * @brief Ptr to server socket object to be used in tests
     */
    std::unique_ptr<networking::TCPSocket> server_ { nullptr };

    /**
     * @brief Address of server socket
     */
    std::string server_addr_ {};

    /**
     * @brief Port of server socket
     */
    uint16_t server_port_ {};

    /**
     * @brief Server sockaddr struct, used for accepting/etc
     */
    sockaddr_in server_sockaddr_ {};

    /**
     * @brief Accept a connection from a client on the server socket
     * @returns std::optional<networking::TCPSocket> client socket to communicate with client
     */
    std::unique_ptr<networking::TCPSocket> acceptClient();

    /**
     * @brief Read data from server
     */
    std::optional<int> readFromServer(networking::TCPSocket& server_client_socket, char* buf, size_t len);
};