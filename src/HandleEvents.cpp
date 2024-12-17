/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleEvents.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 10:00:34 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/17 09:54:32 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../inc/core_server.hpp"

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
    // clients[fd_c].set_response(":wesuseARCH.com NOTICE * :Please authenticate with PASS, then set your NICK and USER.\r\n");

}


void    CoreServer::ReadEvent(int fd)
{
    char buffer[1024];
    int readed = read(fd,buffer,1024);
    if (readed <= 0)
    {
        std::cout << "closing connection FD:" << fd << std::endl;
        fds.erase(std::remove_if(fds.begin(), fds.end(), [fd](const struct pollfd& pfd) { return pfd.fd == fd; }), fds.end());
        close(fd);
        clients.erase(fd);
        return ;
        // continue;
    }
    else if (clients[fd].get_bool() == false)
    {
        clients[fd].authenticate();
    }
    else
    {
        std::string _cmd(buffer);
        // handleCommands(fd,_cmd);
        printf("client %d: %s",fd,buffer);
    }
    std::memset(buffer,0,1024);
}

void    CoreServer::WriteEvent(int fd)
{
    int k  = 0;
    if (!clients[fd].get_response().empty())
    {
        k = write (fd,clients[fd].get_response().c_str(),clients[fd].get_response().length());
        if (k < 0)
        {
            std::cout << "writing error" << std::endl;
            // return ;
        }
        clients[fd].get_response().erase();
     }
}
