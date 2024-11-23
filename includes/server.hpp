/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 04:43:58 by mboutuil          #+#    #+#             */
/*   Updated: 2024/11/23 05:33:04 by mboutuil         ###   ########.fr       */
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


class _client
{
    private :
        int fd_client;
        bool auth;
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        std::string buff;
        struct sockaddr_in client_infos;
    public :
    _client()
    {}
    bool get_bool()
    {
        return auth;
    }
    void    set_bool(bool i)
    {
        auth = i;
    }
    _client (int fd, struct sockaddr_in ddr) : fd_client(fd),client_infos(ddr)
    {    
    }
        int get_fd()
        {
            return fd_client;
        }
        void    set_fd(int fd)
        {
            fd_client = fd;
        }
        std::string get_ip()
        {
            return ip_addr;
        }
        void    set_ip(std::string ip)
        {
            ip_addr = ip;
        }
        void    set_user(std::string ip)
        {
            fullname = ip;
        }
        std::string    get_user()
        {
            return fullname;
        }
        void    set_nick(std::string ip)
        {
            nickname = ip;
        }
        std::string    get_nick()
        {
            return nickname;
        }
        void    set_pass(std::string ip)
        {
            password = ip;
        }
        std::string    get_pass()
        {
            return password;
        }
        std::string get_buff()
        {
            return buff;
        }
        void    set_buff(std::string _buff)
        {
            buff += _buff;
        }
        struct sockaddr_in &get_info()
        {
            return client_infos;
        }
        void    set_info(struct sockaddr_in info)
        {
            client_infos = info;
        }
};

class Core_Server
{
    private:
        std::string ip_addr;
        int _kq;
        struct kevent _ev_set;
        int port;
        int _socket;
        int epoll_fd;
        std::map<int , _client> clients;
        struct sockaddr_in _server_addr;
        void    create_socket();
        void    non_blocking_sock();
        void    bind_sock();
        void    start_listening();
        void    handle_clients();
        void    handle_read_events();
        void    handle_write_events();
    public:
    Core_Server (std::string ip,int _port): ip_addr(ip) ,port(_port)  {}
    // ~Core_Server ();
    void    start_server();
};

#endif