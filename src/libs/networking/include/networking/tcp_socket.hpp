#pragma once
/**
 * @file tcp_socket.hpp
 * @brief Fast TCP sockets
 * @copyright Copyright (c) 2023
 */

#include <functional>
#include "socket_utils.hpp"
#include "logger/logger.hpp"
#include "utils/time.hpp"

namespace networking {
    constexpr size_t BUFFER_SIZE { 64 * 1024 * 1024 };

    class TCPSocket;

    /**
     * @brief Simple default callback used with socket that just
     * logs that an invocation occurred.
     * @param s Socket object
     * @param rx_time Receive time
     */
    void defaultRecvSocketCallback(TCPSocket* s, utils::Nanos rx_time) noexcept;

    class TCPSocket {
    public:

        explicit TCPSocket(logger::Logger& logger);
        ~TCPSocket();

        // Delete default, copy/move ctors, assignment operators
        TCPSocket() = delete;

        TCPSocket(const TCPSocket&) = delete;
        TCPSocket& operator=(const TCPSocket&) = delete;

        TCPSocket(TCPSocket&&) = delete;
        TCPSocket& operator=(TCPSocket&&) = delete;
     
        /**
         * @brief File descriptor of the socket. Default to -1.
         */
        int fd_ { -1 };

        /**
         * @brief Pointer to character buffer for sending data, and the next index up to which valid data 
         * has been written and can be sent.
         */
        std::unique_ptr<char[]> send_buf_ { nullptr };
        size_t next_send_valid_index_ { 0 };

        /**
         * @brief Pointer to recieve buffer for recieving data, and the next index up to which valid data
         * has been recieved and can be read.
         */
        std::unique_ptr<char[]> recv_buf_ { nullptr };
        size_t next_rcv_valid_index_ { 0 };
        
        /**
         * @brief Two flags to indicate if either side of the TCP connection has been closed.
         */
        bool send_disconnected_ { false };
        bool recv_disconnected_ { false };

        struct sockaddr_in inAddr_;

        /**
         * @brief Function to invoke whenever data is recieved on this socket.
         * Callback should take a pointer to the socket and the time, in ns, at which the data was recieved.
         * By default, is set to a callback that just logs its invocation.
         */
        std::function<void(TCPSocket* s, utils::Nanos rx_time)> recv_callback_ { defaultRecvSocketCallback };

        std::string time_str_;

        logger::Logger& logger_;

        /**
         * @brief Destroys the socket - closes the socket descriptor and sets the socket 
         * descriptor to -1.
         */
        void destroy();

        /**
         * @brief Connect to a remote host, initialize TCP socket structure.
         * Destroys existing socket if one exists.
         * @param ip IP to connect to
         * @param iface Interface to use
         * @param port Port to connect to 
         * @param is_listening If creating a listening socket
         * @return int -1 is failure, otherwise socket descriptor of new socket
         */
        int connect(const std::string& ip, const std::string& iface, int port, bool is_listening);

        /**
         * @brief Send data over the socket. Copies the provided data into the send buffer.
         * @param data Data to send
         * @param len Length of the data
         * @return int -1 on failure, otherwise number of bytes queued to be sent
         */
        int send(const void* data, size_t len) noexcept;

        /**
         * @brief Reads available data into receive buffer, icnrement counters, and invoke callback 
         * if any data was read. Also writes out data in send_buffer (that came from send) to the wire and updates counters.
         * @return true Data was received  
         * @return false No data was received
         */
        bool sendAndRecv() noexcept;

    };
}