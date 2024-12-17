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

// Add this typedef for the command handler function pointer
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
        std::vector<>
        // Add the commands map
        std::map<std::string, CommandHandler> commands;
        
        void    create_socket();
        void    start_listening();
        void    start_server();
        
        // Add individual command handler methods
        void    cmdNick(int fd, std::string& args);
        void    cmdUser(int fd, std::string& args);
        void    cmdJoin(int fd, std::string& args);
        void    cmdPrivmsg(int fd, std::string& args);
        // Add more command handlers as needed
        
    public :
        CoreServer(std::string port,std::string _passwd);
        void    WelcomeClient();
        void    WriteEvent(int fd);
        void    handleCommands(int fd, std::string _cmd);
        void    ReadEvent(int fd);
};

#endif