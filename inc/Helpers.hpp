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

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define RESET "\033[0m"

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
    ERROR
} level;

// Server data structure
struct ServerData {
    std::string Passwd;
    int _socket;
    int _poll;
    int Port;
    struct sockaddr_in ServAddr;
    
    level _infoLevel;
    ServerData() : _socket(-1), _poll(-1), Port(-1), _infoLevel(INFO) {
        std::memset(&ServAddr, 0, sizeof(ServAddr));
    }
};



#endif
