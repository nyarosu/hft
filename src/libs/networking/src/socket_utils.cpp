/**
 * @file sockets.cpp
 * @brief Util methods for working with sockets
 * @copyright Copyright (c) 2023
 */

#include "networking/socket_utils.hpp"
#include "utils/time.hpp"

#include <memory>
#include <cstring>

namespace networking {
    std::string getInterfaceIP(const std::string& interfaceName) {
        char buf[NI_MAXHOST] = {'\0'};
        ifaddrs* ifaddr { nullptr };
        
        if (getifaddrs(&ifaddr) != -1) {
            for (ifaddrs* head { ifaddr }; head; head = head->ifa_next) {
                if (head->ifa_addr && head->ifa_addr->sa_family == AF_INET && interfaceName == head->ifa_name) {
                    getnameinfo(head->ifa_addr, sizeof(sockaddr_in), buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
                    break;
                }
            }
            freeifaddrs(ifaddr);
        }   
        return buf;
    }

    bool setNonBlocking(int fd) {
        // Check if the socket is already non-blocking, and if not, set it to non-blocking
        const auto flags { fcntl(fd, F_GETFL, 0) };

        // Invalid socket descriptor
        if (flags == -1) {
            return false;
        }

        // Already non-blocking
        if (flags & O_NONBLOCK) {
            return true;
        }

        return (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1);
    }

    bool setNoDelay(int fd) {
        int one { 1 };
        return (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void *>(&one), sizeof(one)) != -1 );
    }

    bool wouldBlock() {
        return (errno == EWOULDBLOCK || errno == EINPROGRESS);
    }

    bool setTTL(int fd, int ttl) {
        return (setsockopt(fd, IPPROTO_TCP, IP_TTL, reinterpret_cast<void *>(&ttl), sizeof(ttl)) != -1);
    }

    bool setMulticastTTL(int fd, int ttl) {
        return (setsockopt(fd, IPPROTO_TCP, IP_MULTICAST_TTL, reinterpret_cast<void *>(&ttl), sizeof(ttl)) != -1);
    }

    bool setSOTimestamp(int fd) {
        int one { 1 };
        return (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void *>(&one), sizeof(one)) != -1);
    }

    int createSocket(logger::Logger& logger, const std::string& ip, 
            const std::string &iface, int port, bool is_udp,
            bool is_blocking, bool is_listening, 
            int ttl, bool needs_so_timestamp ) {
                
        // Set up logging
        logger.setPrefix("Socket creation: ");
        
        // Will have time written to later
        std::string time_str;

        const auto ip_address { ip.empty() ? getInterfaceIP(iface) : ip };

        logger.log("%:% %() % Creating socket ip: % interface: % port: % is_udp: % is_blocking: % is_listening: % ttl: % SO_time:%\n", 
        __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), ip, iface, port, 
        is_udp, is_blocking, is_listening, ttl, needs_so_timestamp);

        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = is_udp ? SOCK_DGRAM : SOCK_STREAM;
        hints.ai_protocol = is_udp ? IPPROTO_UDP : IPPROTO_TCP;
        hints.ai_flags = is_listening ? AI_PASSIVE : 0;

        // Check if we have an ip address or a hostname
        if (std::isdigit(ip_address.at(0))) {
            hints.ai_flags |= AI_NUMERICHOST;
        }
        hints.ai_flags |= AI_NUMERICSERV;

        addrinfo* result { nullptr };
        const auto res { getaddrinfo(ip_address.c_str(), std::to_string(port).c_str(), &hints, &result) };

        // Getaddrinfo returns 0 on success (so any non-zero, ie truthy, value is an error)
        if (res) {
            logger.log("%:% %() % Getaddrinfo error: % errno: %\n", __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), gai_strerror(res), strerror(errno));
            return -1;
        }
        
        struct addrInfoDeleter {
            void operator()(addrinfo* ptr) const {
                freeaddrinfo(ptr);
            }
        };
        std::unique_ptr<addrinfo, addrInfoDeleter> addrInfo { result };

        // Walk through all getaddrinfo results and create a socket for the first one we can
        int fd { -1 };
        int one { 1 };

        for (addrinfo *rp = addrInfo.get(); rp; rp = rp->ai_next) {
            fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (fd == -1) {
                logger.log("%:% %() % Error when attempting to create socket, errno: %\n", __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
                return -1;
            }
        }

        // Set non blocking, disable Nagle's algorithm
        if (!is_blocking) {
            if (!setNonBlocking(fd)) {
                logger.log("%:% %() % Error when setting non blocking, errno: %\n", __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
                return -1;
            }
        }

        if (!is_udp && !setNoDelay(fd)) {
            logger.log("%:% %() % Error when setting no delay, errno: %\n", __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
            return -1;
        }

        // Now, we either connect or bind, depending on parameters
        if (!is_listening && connect(fd, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1 && !wouldBlock()) {
            logger.log("%:% %() % Error when connecting, errno: %\n", 
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
            return -1;
        }

        if (is_listening && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&one), sizeof(one)) == -1) {
            logger.log("%:% %() % Error when setting SO_REUSEADDR, errno: %\n", 
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
            return -1;
        }

        if (is_listening && bind(fd, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1) {
            logger.log("%:% %() % Error when binding, errno: %\n", 
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
            return -1;
        }

        if (!is_udp && is_listening && listen(fd, MAX_QUEUED_CONNECTIONS) == -1) {
            logger.log("%:% %() % Error when listening, errno: %\n", 
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
            return -1;
        }
        
        // Set TTL, ensuring to set multicast TTL if we're using multicast. Also set SO_TIMESTAMP if we need it
        if (is_udp && ttl) {
            const bool is_multicast { static_cast<bool>(atoi(ip.c_str()) & 0xe0) };
            if (is_multicast && !setMulticastTTL(fd, ttl)) {
                logger.log("%:% %() % Error when setting multicast TTL, errno: %\n", 
                __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
                return -1;
            } 
            if (!is_multicast && !setTTL(fd, ttl)) {
                logger.log("%:% %() % Error when setting TTL, errno: %\n", 
                __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
                return -1;
            }

        }
        
        if (needs_so_timestamp && !setSOTimestamp(fd)) {
            logger.log("%:% %() % Error when setting SO timestamp, errno: %\n", 
            __FILE__, __LINE__, __FUNCTION__, utils::getCurrentTimeStr(time_str), strerror(errno));
            return -1;
        }

        // Clear the prefix we set on the logger
        logger.clearPrefix();

        return fd;
    }
}