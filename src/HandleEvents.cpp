/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleEvents.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 10:00:34 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/02 10:01:08 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/core_server.hpp"

void    CoreServer::WelcomeClient()
{
    int fd_c;
    fd_c = accept(ServData._socket,NULL,NULL);
      if (fd_c < 0)
        { 
            std::cout << "failure connencting client" << std::endl;
            return ;
        }
     else
        {
             std::cout << "client accepted FD:" << fd_c << std::endl;
        }
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        getpeername(fd_c, (struct sockaddr*)&client_addr, &client_len);
        clients[fd_c] = _client(fd_c,client_addr);

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        getnameinfo((struct sockaddr*)&clients[fd_c].get_info(), sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        clients[fd_c].set_response(":wesuseARCH.com NOTICE * :Please authenticate with PASS, then set your NICK and USER.\r\n");
}

void    CoreServer::ReadEvent(int fd)
{
    char buffer[1024];
    int readed = read(fd,buffer,1024);
    if (readed < 0)
    {
        std::cout << "closing connection FD:" << fd << std::endl;
        close(fd);
        clients.erase(fd);
        return ;
        // continue;
    }
    else
        printf("%s",buffer);
}

void    CoreServer::WriteEvent(int fd)
{
    size_t k  = 0;
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
