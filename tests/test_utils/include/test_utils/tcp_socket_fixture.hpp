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
     * @brief Read data sent to server
     * @param server_client_socket Socket recieved from acceptClient()
     * @param buf Buffer to write received data to
     * @param len Length of data to receive
     * @return std::optional<int> Number of bytes received, or std::nullopt if error
     */
    std::optional<int> readFromServer(networking::TCPSocket& server_client_socket, char* buf, size_t len);

    /**
     * @brief Connects to the test server, and accepts the connection on the server side
     * @returns unique_ptr to server's client socket
     */
    std::unique_ptr<networking::TCPSocket> connectToServer();

    /**
     * @brief Generate a random payload of length len
     * @param len Length of payload to generate
     * @return std::string Random data (that you can send over a socket)
     */
    std::string generateRandomPayload(size_t len);

    /**
     * @brief This is similar to the other readFromServer, but it blocks until data is received
     * If data is not received within a certain timeframe, it will return nullopt.
     * Otherwise, it will return the number of bytes received.
     */
    std::optional<int> readFromServerBlocking(networking::TCPSocket& server_client_socket, char* buf, size_t len);
};