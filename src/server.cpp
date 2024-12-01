/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 05:18:50 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/01 12:30:05 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/core_server.hpp"

void    CoreServer::create_socket()
{
    ServData._socket = socket(AF_INET,SOCK_STREAM,0);
    if (ServData._socket < 0)
    {
        std::cerr << "socket error"<<std::endl;
        exit(1);
    }
    int flags = fcntl(ServData._socket , F_GETFL,0);
    fcntl(ServData._socket,F_SETFL,flags | O_NONBLOCK);
    int optval = 1;
    setsockopt( ServData._socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    ServData.ServAddr.sin_family = AF_INET;
    ServData.ServAddr.sin_port = htons(ServData.Port);
   ServData.ServAddr.sin_addr.s_addr = INADDR_ANY;
}

void    CoreServer::start_listening()
{
    if (listen(ServData._socket,5) < 0)
    {
        std::cout << "error" << std::endl;
        exit (1);
    }
    std::cout << "start listening on port:"<< ServData.Port << "..." << std::endl;
    if (bind(ServData._socket ,(struct sockaddr *)&ServData.ServAddr,sizeof(ServData.ServAddr))< 0)
    {
        std::cout << "error biding the socket" << std::endl;
        exit (1);
    }
}

void    CoreServer::start_server()
{
    fds[0].fd = ServData._socket;
    fds[0].events = POLLIN;

    while (1)
    {
        int ret = poll(fds,1024,5000);
        if (ret < 0)
        {
            std::cout << "poll error" << std::endl;
            break ;
        }
        for (int i = 0;i < 1024;i++)
        {
            if (fds->revents & POLLIN)
            {
                if (fds[i].fd == ServData._socket)
                {
                    WelcomeClient();
                }
                else
                {
                    WriteEvent();
                }
            }
            else
                ReadEvent();
        }
    }
}
int main ()
{
    Core_Server _server("hello",6667);

    _server.start_server();

}