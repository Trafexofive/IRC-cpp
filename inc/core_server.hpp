/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 11:39:28 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/02 09:40:31 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

#include "server.hpp"

struct ServerData
{
    std::string         Passwd;
    int                 _socket;
    int                 _poll;
    int                 Port;
    struct sockaddr_in  ServAddr;
};

class CoreServer
{
    private :
        struct ServerData       ServData;
        std::map<int ,_client>  clients;
        struct pollfd fds[1024];
        void    create_socket();
        void    start_listening();
        void    start_server();
    public :
        CoreServer(std::string port,std::string _passwd);
        void    WelcomeClient();
        void    WriteEvent(int fd);
        void    ReadEvent(int fd);
};

#endif