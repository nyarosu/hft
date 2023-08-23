/**
 * @file sockets.cpp
 * @brief Util methods for working with sockets
 * @copyright Copyright (c) 2023
 */

#include "networking/sockets.hpp"

namespace utils::networking {
    std::string getInterfaceIP(const std::string& interfaceName) {
        char buf[NI_MAXHOST] = {'\0'};
        return std::string { "abc" };
    }
}