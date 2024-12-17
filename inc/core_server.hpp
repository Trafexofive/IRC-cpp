/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/01 11:39:28 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/17 10:48:19 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

#include "server.hpp"

typedef void (CoreServer::*CommandHandler)(int fd, std::string& args);

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
        std::map<int, _client> clients;
        std::vector<struct pollfd> fds;
        std::vector<Channels> channels;
        std::map<std::string, CommandHandler> commands;
        
        void    create_socket();
        void    start_listening();
        void    start_server();
        void    cmdNick(int fd, std::vector<std::string> &args);
        void    cmdUser(int fd, std::vector<std::string> &args);
        void    cmdPass(int fd, std::vector<std::string> &args);
        void    cmdJoin(int fd, std::vector<std::string> &args);
        void    cmdPrivmsg(int fd, std::vector<std::string> &args);
    public :
        CoreServer(std::string port,std::string _passwd);
        void    WelcomeClient();
        void    WriteEvent(int fd);
        void    handleCommands(int fd, std::string _cmd);
        void    ReadEvent(int fd);
};

#endif