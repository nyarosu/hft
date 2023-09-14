/**
 * @file tcp_server.cpp
 * @brief TCP server class, connects and manages clients
 * @copyright Copyright (c) 2023
 */

#include "networking/tcp_server.hpp"
#include <cstring>

namespace networking {
    void TCPServer::defaultRecvCallback(TCPSocket* s, utils::Nanos rx_time) noexcept {
        logger_.log("%:% %() % TCPServer::defaultRecvServerCallback() socket:% len:% rx:%\n", 
            __FILE__, __LINE__, __FUNCTION__, 
            utils::getCurrentTimeStr(time_str_), s->fd_, s->next_rcv_valid_index_, rx_time);
    }

    void TCPServer::defaultRecvFinishedCallback() noexcept {
        logger_.log("%:% %() % TCPServer::defaultRecvFinishedCallback()\n", __FILE__, __LINE__,
            __FUNCTION__, utils::getCurrentTimeStr(time_str_));
    }

    TCPServer::TCPServer(logger::Logger& logger)
        : listener_socket_ { logger }, logger_ { logger }  {
            recv_callback_ = [this](auto socket, auto rx_time) {
                defaultRecvCallback(socket, rx_time);
            };
            recv_finished_callback_ = [this]() {
                defaultRecvFinishedCallback();
            };
    }

    void TCPServer::destroy() {
        close(efd_);
        efd_ = -1;
        listener_socket_.destroy();
        for (auto socket : sockets_) {
            socket->destroy();
            delete socket;
        }
    }

    TCPServer::~TCPServer() {
        destroy();

    }

    void TCPServer::listen(const std::string& interface, int port) {
        // Remove any existing server
        destroy();

        efd_ = epoll_create(1);
        utils::ASSERT(efd_ >= 0, "epoll_create() failed, error:" + std::string(std::strerror(errno)));
        utils::ASSERT(listener_socket_.connect("", interface, port, true) >= 0, 
            "Listener socket failed to connect. Error: " + std::string(std::strerror(errno)));
        
        utils::ASSERT(addToEpollList(&listener_socket_), "epoll_add() failed for listener socket. Error: " + std::string(std::strerror(errno)));
    }

    int TCPServer::listen(const std::string& interface) {
        // Remove any existing server
        destroy();

        efd_ = epoll_create(1);
        utils::ASSERT(efd_ >= 0, "epoll_create() failed, error:" + std::string(std::strerror(errno)));

        int fd { listener_socket_.connect("", interface, 0, true) };
        utils::ASSERT(fd >= 0, "Listener socket failed to connect. Error: " + std::string(std::strerror(errno)));

        // Find out what port was assigned by the kernel
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(0);

        socklen_t len = sizeof(addr);
        int ret = getsockname(fd, (struct sockaddr *)&addr, &len);
        utils::ASSERT(ret != -1, "getsockname() failed. Error: " + std::string(std::strerror(errno)));

        utils::ASSERT(addToEpollList(&listener_socket_), "epoll_add() failed for listener socket. Error: " + std::string(std::strerror(errno)));

        return ntohs(addr.sin_port);

    }

    bool TCPServer::addToEpollList(TCPSocket* socket) {
        epoll_event ev{};
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = reinterpret_cast<void *>(socket);
        return (epoll_ctl(efd_, EPOLL_CTL_ADD, socket->fd_, &ev)) != -1;
    }

    bool TCPServer::removeFromEpollList(TCPSocket* socket) {
        return (epoll_ctl(efd_, EPOLL_CTL_DEL, socket->fd_, nullptr)) != -1;
    }

    void TCPServer::del(TCPSocket* socket) {
        removeFromEpollList(socket);

        sockets_.erase(std::remove(sockets_.begin(), sockets_.end(), socket), sockets_.end());
        receive_sockets_.erase(std::remove(receive_sockets_.begin(), receive_sockets_.end(), socket), receive_sockets_.end());
        send_sockets_.erase(std::remove(send_sockets_.begin(), send_sockets_.end(), socket), send_sockets_.end());
    }

    void TCPServer::poll() noexcept {
        const int MAX_EVENTS { static_cast<int>(sockets_.size() + 1) };

        // Clear disconnected sockets
        for (auto& socket : disconnected_sockets_) {
            del(socket);
        }

        const int num_events { epoll_wait(efd_, events_, MAX_EVENTS, 0) };

        // Process events
        bool new_connection { false };
        for (int i { 0 }; i < num_events; i++) {
            epoll_event& event { events_[i] };
            auto socket { reinterpret_cast<TCPSocket*>(event.data.ptr) };
            
            if (event.events & EPOLLIN) {
                // New incoming connection
                if (socket == &listener_socket_) {
                    logger_.log("%:% %() % EPOLLIN New connection on listener:%\n", __FILE__, __LINE__,
                        __FUNCTION__, utils::getCurrentTimeStr(time_str_), socket->fd_);
                    new_connection = true;
                    continue;
                }

                // Data received on existing connection. Make sure this socket is in the receive list.
                logger_.log("%:% %() % EPOLLIN Data received on socket:%\n", __FILE__, __LINE__,
                    __FUNCTION__, utils::getCurrentTimeStr(time_str_), socket->fd_);
                if (std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) == receive_sockets_.end()) {
                    receive_sockets_.push_back(socket);
                }
            }

            if (event.events & EPOLLOUT) {
                // We can send on this socket, let's make sure it's in our send list
                logger_.log("%:% %() % EPOLLOUT Socket:% is ready to send\n", __FILE__, __LINE__,
                    __FUNCTION__, utils::getCurrentTimeStr(time_str_), socket->fd_);
                if (std::find(send_sockets_.begin(), send_sockets_.end(), socket) == send_sockets_.end()) {
                    send_sockets_.push_back(socket);
                }
            }

            // Check for errors or closed connections
            if (event.events & (EPOLLERR | EPOLLHUP)) {
                logger_.log("%:% %() % EPOLLERR or EPOLLHUP on socket:%\n", __FILE__, __LINE__,
                    __FUNCTION__, utils::getCurrentTimeStr(time_str_), socket->fd_);

                if (std::find(disconnected_sockets_.begin(), disconnected_sockets_.end(), socket) == disconnected_sockets_.end()) {
                    disconnected_sockets_.push_back(socket);
                }
            }
        }

            // Accept new connection (if there is one)
            while (new_connection) {
                logger_.log("%:% %() % Accepting new connection on listener:%\n", __FILE__, __LINE__,
                    __FUNCTION__, utils::getCurrentTimeStr(time_str_), listener_socket_.fd_);

                sockaddr_storage remote_addr;
                socklen_t remote_addr_len { sizeof(remote_addr) };

                int incoming_fd { accept(listener_socket_.fd_, reinterpret_cast<sockaddr*>(&remote_addr), &remote_addr_len) };
                if (incoming_fd == -1) {
                    break;
                }

                utils::ASSERT(setNonBlocking(incoming_fd) && setNoDelay(incoming_fd), "Failed to set non-blocking and no-delay on incoming socket: " 
                    + std::to_string(incoming_fd));    
                
                logger_.log("%:% %() % New connection accepted on listener:% new socket:%\n", __FILE__, __LINE__,
                    __FUNCTION__, utils::getCurrentTimeStr(time_str_), listener_socket_.fd_, incoming_fd);
                
                TCPSocket* client { new TCPSocket { logger_ } };
                client->fd_ = incoming_fd;
                client->recv_callback_ = recv_callback_;

                utils::ASSERT(addToEpollList(client), "epoll_add() failed for new client socket. Error: " + std::string(std::strerror(errno)));

                if (std::find(sockets_.begin(), sockets_.end(), client) == sockets_.end()) {
                    sockets_.push_back(client);
                }

                if (std::find(receive_sockets_.begin(), receive_sockets_.end(), client) == receive_sockets_.end()) {
                    receive_sockets_.push_back(client);
                }
            }
        
    }

    void TCPServer::sendAndRecv() noexcept {
        bool recv { false };
        for (auto socket : receive_sockets_) {
            if (socket->sendAndRecv()) {
                recv = true;
            }
        }
        if (recv) { 
            recv_finished_callback_();
        }

        for (auto socket : send_sockets_) {
            socket->sendAndRecv();
        }
    }
}