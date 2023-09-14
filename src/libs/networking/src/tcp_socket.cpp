/**
 * @file tcp_socket.cpp
 * @brief Fast TCP sockets
 * @copyright Copyright (c) 2023
 */

#include "networking/tcp_socket.hpp"
#include <cstring>

namespace networking {
    void defaultRecvSocketCallback(TCPSocket* socket, utils::Nanos rx_time) noexcept {
        socket->logger_.log("%:% %() % TCPSocket::defaultRecvCallback() socket:% len:% rx:%\n",
        __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(socket->time_str_), 
        socket->fd_, socket->next_rcv_valid_index_, rx_time);
    }

    TCPSocket::TCPSocket(logger::Logger& logger)
        : logger_ { logger } {
        send_buf_ = std::make_unique<char[]>(BUFFER_SIZE);
        recv_buf_ = std::make_unique<char[]>(BUFFER_SIZE);

        recv_callback_ = [this](TCPSocket* socket, utils::Nanos rx_time) {
            defaultRecvSocketCallback(socket, rx_time);
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

    bool TCPSocket::sendAndRecv() noexcept {
        char ctrl_buf[CMSG_SPACE(sizeof(struct timeval))];
        struct cmsghdr *cmsg = (struct cmsghdr *) &ctrl_buf;
        struct iovec iov;

        iov.iov_base = recv_buf_.get() + next_rcv_valid_index_;
        iov.iov_len = BUFFER_SIZE - next_rcv_valid_index_;

        msghdr msg;
        msg.msg_control = ctrl_buf;
        msg.msg_controllen = sizeof(ctrl_buf);
        msg.msg_name = &inAddr_;
        msg.msg_namelen = sizeof(inAddr_);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        const auto n_recv = recvmsg(fd_, &msg, MSG_DONTWAIT);
        logger_.log("%:% %() % recv_attempt:% result:%\n", __FILE__, __LINE__,
            __FUNCTION__, utils::getCurrentTimeStr(time_str_), fd_, n_recv);
        
        if (n_recv < 0) [[unlikely]] {
            logger_.log("%:% %() % error on recv:% strerrno:%\n", __FILE__, __LINE__,
            __FUNCTION__, utils::getCurrentTimeStr(time_str_), fd_, strerror(errno));
        }
        if (n_recv > 0) {
            next_rcv_valid_index_ += n_recv;
            utils::Nanos kernel_time = 0;
            struct timeval time_kernel;
            
            if (cmsg->cmsg_level == SOL_SOCKET &&
                cmsg->cmsg_type == SCM_TIMESTAMP &&
                cmsg->cmsg_len == CMSG_LEN(sizeof(time_kernel))) {
                    memcpy(&time_kernel, CMSG_DATA(cmsg), sizeof(time_kernel));
                    kernel_time = time_kernel.tv_sec * utils::NANOS_TO_SECS +
                                time_kernel.tv_usec * utils::NANOS_TO_MICROS; 
                }
            const auto user_time = utils::getCurrentNanos();
            logger_.log("%:% %() % read_socket:% len:% utime:% ktime:% diff:%\n", 
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str_), 
            fd_, next_rcv_valid_index_, user_time, kernel_time, (user_time - kernel_time));

            recv_callback_(this, kernel_time);
        }

        ssize_t n_send = std::min(BUFFER_SIZE, next_send_valid_index_);
        
        while (n_send > 0) {
            auto n_send_this_msg = std::min(static_cast<ssize_t>(next_send_valid_index_), n_send);
            const int flags = MSG_DONTWAIT | MSG_NOSIGNAL | (n_send_this_msg < n_send ? MSG_MORE : 0);
            auto n = ::send(fd_, send_buf_.get(), n_send_this_msg, flags);

            if (n < 0) [[unlikely]] {
                if (!wouldBlock()) {
                    send_disconnected_ = true;
                }
                break;
            }
            logger_.log("%:% %() % send_socket:% len:%\n", __FILE__, __LINE__,
            __FUNCTION__, utils::getCurrentTimeStr(time_str_), fd_, n);

            n_send -= n;
            utils::ASSERT(n == n_send_this_msg, "Cannot send partial message");
        }
        next_send_valid_index_ = 0;
        return (n_recv > 0);
    }
}