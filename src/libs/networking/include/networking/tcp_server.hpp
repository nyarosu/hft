#pragma once
/**
 * @file tcp_server.hpp
 * @brief TCP server class, connects and manages clients
 * @copyright Copyright (c) 2023
 */

#include "networking/tcp_socket.hpp"
#include "logger/logger.hpp"

namespace networking {
    constexpr int MAX_EVENTS { 1024 };

    
    class TCPServer {

    public:
        /**
         * @brief Default callback that does nothing but log the invocation. Just a placeholder.
         * @param s socket 
         * @param rx_time received time
         */
        void defaultRecvCallback(TCPSocket* s, utils::Nanos rx_time) noexcept;

        /**
         * @brief Default callback for poll loop finished. Just a placeholder.
         */
        void defaultRecvFinishedCallback() noexcept;

        /**
         * @brief Create a new TCP server.
         * @param logger Logger object that TCPServer and its sockets will write to
         */
        explicit TCPServer(logger::Logger& logger);

        /**
         * @brief Destroy the TCP server.
        */
        void destroy();

        ~TCPServer();

        /**
         * @brief Start listening for new connections
         * @param interface to bind on
         * @param port to bind on
         */
        void listen(const std::string& interface, int port);

        /**
         * @brief Start listening for new connections - on any port on the given interface.
         * Picks an available port at random, returns the port that was picked.
         * @param interface Interface to listen on.
         */
        int listen(const std::string& interface);

        /**
         * @brief Poll. This first checks for new connections (and if so, adds them to our vectors)
         * Then, we detect sockets that have disconnected from client side and remove them.
         * Finally, we check to see if there are sockets with data ready to be read or with outgoing data
         */
        void poll() noexcept;

        /**
         * @brief Remove socket from the list of monitored sockets
         * @param socket socket to remove
         */
        void del(TCPSocket* socket);

        /**
         * @brief Send and receive data on all sockets
         */
        void sendAndRecv() noexcept;

        // Delete default ctor, copy/move ctor, copy/move assignment
        TCPServer() = delete;

        TCPServer(const TCPServer&) = delete;
        TCPServer(TCPServer&&) = delete;
        
        TCPServer& operator=(const TCPServer&) = delete;
        TCPServer& operator=(TCPServer&&) = delete;

        /**
         * @brief epoll file descriptor
         */
        int efd_ { -1 };

        /**
         * @brief Listener socket that listens for incoming connections
         */
        networking::TCPSocket listener_socket_;

        /**
         * @brief Array of events to monitor the listening socket and connected clients
         */
        epoll_event events_[MAX_EVENTS];

        /**
         * @brief Arrays of sockets that we expect to receive data from, sockets we expect to send on, and sockets
         * that are disconnected.
         */
        std::vector<TCPSocket*> sockets_, receive_sockets_, 
            send_sockets_, disconnected_sockets_;
        
        /**
         * @brief Callback to be invoked when data is received on a socket
         * @param s socket that received data
         * @param rx_time time that data was received (this is to make sure
         * data is processed in the order it was received).
         */
        std::function<void(TCPSocket* s, utils::Nanos rx_time)> recv_callback_;

        /**
         * @brief Callback to be invoked when all callbacks in the current
         * poll loop have been completed.
         */
        std::function<void()> recv_finished_callback_;

        std::string time_str_;

        logger::Logger& logger_;

    private:
        bool addToEpollList(TCPSocket* socket);
        bool removeFromEpollList(TCPSocket* socket);
    };
}