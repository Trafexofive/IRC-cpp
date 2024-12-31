/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */ /*   HandleEvents.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 10:44:02 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 10:44:02 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <netdb.h>
#include <cstdio>
#include <sstream>
#include <algorithm>

class FdRemovePredicate
{
    private:
        int fd_to_remove;
    public:
        FdRemovePredicate(int fd) : fd_to_remove(fd) {}
        bool operator()(const struct pollfd& pfd) const
        {
            return pfd.fd == fd_to_remove;
        }
};

void    CoreServer::WelcomeClient()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int fd_c = accept(ServData._socket, (struct sockaddr*)&client_addr, &client_len);
    
    if (fd_c < 0)
    { 
        std::cout << formatServerMessage("ERROR", "Failure connecting client") << std::endl;
        return;
    }
    
    std::ostringstream oss;
    oss << "Client accepted FD: " << fd_c;
    std::cout << formatServerMessage("INFO", oss.str()) << std::endl;
    
    clients[fd_c] = _client(fd_c, client_addr);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    getnameinfo((struct sockaddr*)&clients[fd_c].get_info(), sizeof(client_addr), 
                host, NI_MAXHOST, service, NI_MAXSERV, 
                NI_NUMERICHOST | NI_NUMERICSERV);

    struct pollfd _fd;
    _fd.fd = fd_c;
    _fd.events = POLLIN;
    fds.push_back(_fd);
}

void CoreServer::handleCommand(int fd, const std::string& line)
{
    std::istringstream iss(line);
    std::vector<std::string> args;
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    if (args.empty()) {
        return;
    }
    
    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    
    std::cout << formatServerMessage("DEBUG", "Processing command: " + command) << std::endl;
    
    if (commands.find(command) != commands.end()) {
        (this->*commands[command])(fd, args);
        WriteEvent(fd);  // Send response immediately after command
    }
}


void CoreServer::WriteEvent(int fd)
{
    if (!clients[fd].get_response().empty())
    {
        const std::string& response = clients[fd].get_response();
        std::cout << formatServerMessage("DEBUG", "Sending response: " + response) << std::endl;
        
        ssize_t written = send(fd, response.c_str(), response.length(), 0);
        
        if (written < 0)
        {
            std::cout << formatServerMessage("ERROR", "Writing error") << std::endl;
        }
        else
        {
            clients[fd].clear_response();
        }
    }
}

void CoreServer::ReadEvent(int fd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    
    int readed = read(fd, buffer, sizeof(buffer) - 1);
    
    if (readed <= 0)
    {
        std::ostringstream oss;
        oss << "Closing connection FD: " << fd;
        std::cout << formatServerMessage("INFO", oss.str()) << std::endl;
        
        std::vector<struct pollfd>::iterator new_end = 
            std::remove_if(fds.begin(), fds.end(), FdRemovePredicate(fd));
        fds.erase(new_end, fds.end());
        close(fd);
        clients.erase(fd);
        return;
    }
    
    buffer[readed] = '\0';
    std::string input(buffer);
    std::istringstream iss(input);
    std::string line;
    
    while (std::getline(iss, line))
    {
        // Remove \r if present
        if (!line.empty() && line[line.length()-1] == '\r')
            line.erase(line.length()-1);
        
        if (line.empty())
            continue;
            
        handleCommand(fd, line);
    }
}

