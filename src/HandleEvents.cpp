/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleEvents.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 03:39:13 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 03:39:13 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/core_server.hpp"
#include <netdb.h>
#include <cstdio>

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
        std::cout << "failure connecting client" << std::endl;
        return;
    }
    
    std::cout << "client accepted FD:" << fd_c << std::endl;
    
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

void    CoreServer::ReadEvent(int fd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    
    int readed = read(fd, buffer, sizeof(buffer) - 1); // Leave space for null terminator
    
    if (readed <= 0)
    {
        std::cout << "closing connection FD:" << fd << std::endl;
        std::vector<struct pollfd>::iterator new_end = 
            std::remove_if(fds.begin(), fds.end(), FdRemovePredicate(fd));
        fds.erase(new_end, fds.end());
        close(fd);
        clients.erase(fd);
        return;
    }
    
    buffer[readed] = '\0'; // Ensure null termination
    
    if (clients[fd].get_bool() == false)
    {
        clients[fd].authenticate();
    }
    else
    {
        std::string _cmd(buffer);
        // handleCommands(fd,_cmd);
        printf("client %d: %s", fd, buffer);
    }
}

void    CoreServer::WriteEvent(int fd)
{
    if (!clients[fd].get_response().empty())
    {
        const std::string& response = clients[fd].get_response();
        int k = write(fd, response.c_str(), response.length());
        
        if (k < 0)
        {
            std::cout << "writing error" << std::endl;
        }
        clients[fd].get_response().erase();
    }
}
