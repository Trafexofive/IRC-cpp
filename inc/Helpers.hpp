/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helpers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 17:05:55 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/31 17:05:55 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERS_HPP
#define HELPERS_HPP

#define RESET "\033[0m"

#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define BLUE "\033[34m"


#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
// System includes
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

typedef enum {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
} LEVEL;

// Server data structure
struct ServerData {
    std::string Passwd;
    int _socket;
    int _poll;
    int Port;
    std::string Host;
    // std::string Motd;
    // std::string Welcome;
    std::string ServerName;
    LEVEL _infoLevel;

    struct sockaddr_in ServAddr;
    
    ServerData() : Passwd("password"), _socket(-1), _poll(-1), Port(6667), Host("morpheus-server.ddns.net"), ServerName("WeUseArch"), _infoLevel(INFO) {
        memset(&ServAddr, 0, sizeof(ServAddr));
    }
};



#endif
