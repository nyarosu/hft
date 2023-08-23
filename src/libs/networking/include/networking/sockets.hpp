#pragma once
/**
 * @file sockets.hpp
 * @brief Utils for working with sockets
 * @copyright Copyright (c) 2023
 */

#include <string>
#include <unordered_set>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "logger/logger.hpp"

namespace utils::networking {
    /**
     * @brief Maximum connections that can be waiting in listen() queue
     */
    constexpr int MAX_QUEUED_CONNECTIONS { 1024 };
    
    /**
     * @brief Get the IP address associated with the given interface
     * @param interfaceName 
     * @return std::string IP address as string
     */
    std::string getInterfaceIP(const std::string& interfaceName);

    /**
     * @brief Set the socket
     * @param interfaceName 
     * @return bool Success
     */
    bool setNonBlocking(int fd);

    /**
     * @brief Disable Nagle's algorithm
     * @param fd Socket descriptor
     * @return bool Success
     */
    bool setNoDelay(int fd);

    /**
     * @brief Enable/disable receiving timestamps on incoming packets
     * @param fd Socket descriptor that will be receiving packets
     * @return bool Success
     */
    bool setSOTimestamp(int fd);

    bool wouldBlock();

    /**
     * @brief Set TTL for multicast packets sent from FD.
     * (TTL limits the number of hops a packet can make before being discarded)
     * @param fd Socket descriptor of packet sender
     * @param ttl TTL to set
     * @return bool Success
     */
    bool setMulticastTTL(int fd, int ttl);

    /**
     * @brief Set TTL for unicast packets sent from FD.
     * @param fd socket descriptor
     * @param ttl TTL to set
     * @return bool Success
     */
    bool setTTL(int fd, int ttl);

    /**
     * @brief Join multicast group
     * @param fd Socket descriptor
     * @param ip IP address of multicast group
     * @param iface Interface to join multicast group on
     * @param port Port of multicast group
     * @return bool Success 
     */
    bool join(int fd, const std::string& ip, const std::string& iface, int port);

    /**
     * @brief Create a Socket object
     * 
     * @param logger Used to log any errors/warnings, or success
     * @param ip target IP
     * @param iface network interface to use
     * @param port Port number to bind/connect to
     * @param is_udp if creating a UDP socket
     * @param is_blocking if creating a blocking socket
     * @param is_listening if creating a listening socket
     * @param ttl time to live
     * @param needs_so_timestamp if you need the timestamp of incoming packets
     * @return int socket descriptor
     */
    int createSocket(logger::Logger& logger, const std::string& ip, 
        const std::string &iface, int port, bool is_udp,
        bool is_blocking, bool is_listening, 
        int ttl, bool needs_so_timestamp );

}