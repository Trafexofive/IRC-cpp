/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 04:43:58 by mboutuil          #+#    #+#             */
/*   Updated: 2024/11/22 21:17:13 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include<unistd.h>
#include<sys/syslimits.h>
// #include<stdlib.h>
#include<fstream>
#include<string>
#include<map>
#include<vector>
#include <netdb.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include <iostream>
// #include <sys/epoll.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#define MAX_EVENT 1000
/*
Clients must have a unique name maximum 9 characters
the servers must know the client name and the client host and wich server
is addressed to ==> will be considered in the client class
also there is special clients wich are the operators

*/


class client
{
    private :
        // int fd_client;
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        struct sockaddr_in client_infos;
    public :
        int get_fd();
        void    set_fd(int fd);
        std::string get_ip();
        void    set_ip(std::string ip);
        void    set_user(std::string ip);
        std::string    get_user();
        void    set_nick(std::string ip);
        std::string    get_nick();
        void    set_pass(std::string ip);
        std::string    get_pass();
};

class Core_Server
{
    private:
        std::string ip_addr;
        int port;
        int _socket;
        int epoll_fd;
        struct sockaddr_in _server_addr;
        void    create_socket();
        void    non_blocking_sock();
        void    bind_sock();
        void    start_listening();
        void    handle_clients();
    public:
    Core_Server (std::string ip,int _port): ip_addr(ip) ,port(_port)  {}
    // ~Core_Server ();
    void    start_server();
};

#endif