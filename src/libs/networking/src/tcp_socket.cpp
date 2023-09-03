/**
 * @file tcp_socket.cpp
 * @brief Fast TCP sockets
 * @copyright Copyright (c) 2023
 */

#include "networking/tcp_socket.hpp"
#include <cstring>

namespace networking {
    void defaultRecvCallback(TCPSocket* socket, utils::Nanos rx_time) noexcept {
        socket->logger_.log("%:% %() % TCPSocket::defaultRecvCallback() socket:% len:% rx:%\n",
        __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(socket->time_str_), 
        socket->fd_, socket->next_rcv_valid_index_, rx_time);
    }

    TCPSocket::TCPSocket(logger::Logger& logger)
        : logger_ { logger } {
        send_buf_ = std::make_unique<char[]>(BUFFER_SIZE);
        recv_buf_ = std::make_unique<char[]>(BUFFER_SIZE);

        recv_callback_ = [this](TCPSocket* socket, utils::Nanos rx_time) {
            defaultRecvCallback(socket, rx_time);
        };
    }

    void TCPSocket::destroy() {
        close(fd_);
        fd_ = -1;
    }

    TCPSocket::~TCPSocket() {
        destroy();
        send_buf_ = nullptr;
        recv_buf_ = nullptr;
    }

    int TCPSocket::connect(const std::string& ip, const std::string& iface, int port, bool is_listening) {
        // In case of socket reuse, destroy the old socket
        destroy();

        fd_ = createSocket(logger_, ip, iface, port, false, false, is_listening, 0, true);
        inAddr_.sin_addr.s_addr = INADDR_ANY;
        inAddr_.sin_port = htons(port);
        inAddr_.sin_family = AF_INET;

        return fd_;
    }

    int TCPSocket::send(const void* data, size_t len) noexcept {
        // Check that length is valid, and we won't overflow the buffer
        if (len <= 0 || len > (BUFFER_SIZE - next_send_valid_index_)) [[unlikely]] {
            logger_.log("%:% %() % TCPSocket::send() not enough space in buffer:%\n",
            __FILE__, __LINE__, __FUNCTION__, len);
            return -1;
        }

        // Copy data into buffer
        memcpy(send_buf_.get() + next_send_valid_index_, data, len);
        next_send_valid_index_ += len;
        return len;
    }
}