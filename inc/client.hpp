/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 20:56:13 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/07 20:56:13 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include<map>
#include<vector>
#include <functional>
#include <netdb.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include <iostream>
#include<sstream>
// #include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
// #include <sys/event.h>
#include <sys/time.h>
#include <poll.h>

class _client
{
    private :
        int fd_client;
        bool auth;
        bool connected;
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        std::string buff;
        std::string response;
        // bool    completed_cmd;
        // std::vector<std::vector <std::string>> parsed;
        struct sockaddr_in client_infos;
    public :
    _client()
    {}
    bool get_bool()
    {
        return auth;
    }
    bool get_connect()
    {
        return connected;
    }
    void    set_connect(bool i)
    {
        connected = i;
    }
    void    set_bool(bool i)
    {
        auth = i;
    }
    void    set_response(std::string _resp)
    {
        response = _resp;
    }
    std::string    &get_response()
    {
        return response;
    }
    _client (int fd, struct sockaddr_in ddr) : fd_client(fd),auth(false),client_infos(ddr){}
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
        void    set_buff(std::string _buff,int flag)
        {
            if (flag)
            {
                buff += _buff;
            }
            else
                buff = _buff;
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


#endif
