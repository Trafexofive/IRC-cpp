/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 04:27:32 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/28 04:27:32 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <cstdlib>
#include <sstream>

void CoreServer::create_socket() {
    std::cout << formatServerMessage("INFO", "Creating socket...") << std::endl;
    
    // Create socket
    ServData._socket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServData._socket < 0) {
        std::cerr << formatServerMessage("ERROR", "Socket creation failed") << std::endl;
        exit(1);
    }
    
    // Set socket options
    int optval = 1;
    if (setsockopt(ServData._socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        std::cerr << formatServerMessage("ERROR", "Failed to set socket options") << std::endl;
        exit(1);
    }
    
    // Set non-blocking
    int flags = fcntl(ServData._socket, F_GETFL, 0);
    if (flags < 0 || fcntl(ServData._socket, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << formatServerMessage("ERROR", "Failed to set non-blocking") << std::endl;
        exit(1);
    }
    
    // Setup server address
    memset(&ServData.ServAddr, 0, sizeof(ServData.ServAddr));
    ServData.ServAddr.sin_family = AF_INET;
    ServData.ServAddr.sin_addr.s_addr = INADDR_ANY;
    ServData.ServAddr.sin_port = htons(ServData.Port);
    
    // Setup poll fd
    struct pollfd _fd;
    _fd.fd = ServData._socket;
    _fd.events = POLLIN;
    fds.push_back(_fd);
    
    std::cout << formatServerMessage("INFO", "Socket setup complete") << std::endl;
}

void CoreServer::start_listening() {
    std::cout << formatServerMessage("DEBUG", "Binding socket...") << std::endl;
    
    if (bind(ServData._socket, (struct sockaddr *)&ServData.ServAddr, sizeof(ServData.ServAddr)) < 0) {
        std::cerr << formatServerMessage("ERROR", "Binding failed: ") 
                  << strerror(errno) << std::endl;
        exit(1);
    }
    
    std::cout << formatServerMessage("INFO", "Starting to listen...") << std::endl;
    
    if (listen(ServData._socket, SOMAXCONN) < 0) {
        std::cerr << formatServerMessage("ERROR", "Listen failed: ")
                  << strerror(errno) << std::endl;
        exit(1);
    }
    
    std::ostringstream oss;
    oss << "Server listening on port: " << ServData.Port;
    std::cout << formatServerMessage("INFO", oss.str()) << std::endl;
}

void CoreServer::start_server() {
    std::cout << formatServerMessage("DEBUG", "Server main loop starting...") << std::endl;
    
    while (1) {
        int ret = poll(&fds[0], fds.size(), -1);
        if (ret < 0) {
            if (errno == EINTR) continue;  // Handle interruption
            std::cerr << formatServerMessage("ERROR", "Poll failed: ")
                      << strerror(errno) << std::endl;
            break;
        }
        
        for (size_t i = 0; i < fds.size(); i++) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == ServData._socket) {
                    WelcomeClient();
                } else {
                    ReadEvent(fds[i].fd);
                }
            } else if (fds[i].revents & (POLLERR | POLLHUP)) {
                // Handle disconnection/errors
                HandleDisconnect(fds[i].fd);
            }
        }
    }
}

CoreServer::CoreServer(std::string port, std::string password) {
    std::cout << formatServerMessage("INFO", "Constructing Server Class ...") << std::endl;
    
    if (!IsValidPort(port, ServData.Port)) {
        std::cerr << formatServerMessage("ERROR", "Invalid port") << std::endl;
        exit(1);
    }
    
    if (!IsValidPass(password, ServData.Passwd)) {
        std::cerr << formatServerMessage("ERROR", "Invalid Password") << std::endl;
        DisplayPassInfo();
        exit(1);
    }
    
    std::cout << formatServerMessage("INFO", "Registering commands...") << std::endl;
    commands[PASS] = &CoreServer::cmdPass;
    
    create_socket();
    start_listening();
    start_server();
}
