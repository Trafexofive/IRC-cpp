/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 04:43:58 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/02 10:07:27 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include<unistd.h>
// #include<sys/syslimits.h>
// #include<stdlib.h>
#include<fstream>
#include<string>

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
#include<cstring>
#include <algorithm>
#define MAX_EVENT 1000
/*
Clients must have a unique name maximum 9 characters
the servers must know the client name and the client host and wich server
is addressed to ==> will be considered in the client class
also there is special clients wich are the operators

*/
#define RPL_WELCOME 001
#define RPL_YOURHOST 002
#define RPL_CREATED 003
#define RPL_MYINFO 004
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_TOPIC 331
#define ERR_ERRONEUSNICKNAME 332	
#define ERR_NICKNAMEINUSE 433	
#define ERR_NEEDMOREPARAMS 461
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NOORIGIN 409

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

class Channels
{
    private:

    public:
}
// class Core_Server
// {
//     private:
//         std::string passwd;
//         int _kq;
//         struct kevent _ev_set;
//         int port;
//         int _socket;
//         int epoll_fd;
//         std::map<int , _client> clients;
//         // struct sockaddr_in _server_addr;
//         void    create_socket();
//         // void    non_blocking_sock();
//         void    bind_sock();
//         void    start_listening();
//         void    handle_clients();
//         void    process_command(_client &client,std::string buff);

//         void    handle_read_events(int fd)
//         {
//          char buffer[1024];
//          int readed = read(fd,buffer,1024);
//          if (readed <= 0)
//          {
//          EV_SET(&_ev_set,fd ,EVFILT_READ , EV_DELETE ,0,0,NULL);
//              std::cout << "closing connection FD:" << fd << std::endl;
//              close(fd);
//              clients.erase(fd);
//              // continue;
//          }
//          else 
//          {
//          buffer[readed] = 0;
//          std::string _buff(buffer);
//          process_command(clients[fd],_buff);
//         //  std::cout << buffer <<std::endl;
//         // handle
//         //  clients[fd].set_buff(_buff,0);
//          EV_SET(&_ev_set,fd,EVFILT_WRITE,EV_ADD | EV_ENABLE,0,0,NULL);
//          kevent(_kq,&_ev_set,1,NULL,0,NULL);
//          }
//         }
//         void    handle_write_events(int fd)
//         {
//             size_t k  = 0;
//             if (!clients[fd].get_response().empty())
//             {
//                 k = write (fd,clients[fd].get_response().c_str(),clients[fd].get_response().length());
//                 clients[fd].get_response().erase();
//             }
//         //     std::string response = " welcome to my server \r\n";
//         //    size_t k = write (fd,response.c_str(),response.length());
//            EV_SET(&_ev_set,fd ,EVFILT_READ ,EV_ENABLE,0,0,NULL);
//            kevent(_kq,&_ev_set,1,NULL,0,NULL);
//            // int k;
//            if (k < 0)
//            {
//                std::cout << "close _connection from " << fd << std::endl;
//                close(fd);
//                clients.erase(fd);
//                // EV_SET(&_ev_set,fd ,EVFILT_WRITE , EV_DELETE ,0,0,NULL);
//                // kevent(_kq,&_ev_set,1,NULL,0,NULL);
//                // continue;
//            }
//                EV_SET(&_ev_set,fd ,EVFILT_WRITE , EV_DELETE ,0,0,NULL);
//                         kevent(_kq,&_ev_set,1,NULL,0,NULL);
//         }
//     public:
//     Core_Server (std::string ip,int _port): passwd(ip) ,port(_port)  {}
//     std::string send_welcome_message(_client& cl);
//     // ~Core_Server ();
//     void    start_server();
// };

#endif
