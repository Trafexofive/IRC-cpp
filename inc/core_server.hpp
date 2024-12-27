/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Trafexofive <mboutuil@student.42.fr>       +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 01:21:16 by Trafexofive       #+#    #+#           */
/*   Updated: 2024/12/27 01:21:16 by Trafexofive      ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

#include <map>
#include <vector>
#include <string>
#include <cstring>      // for memset
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // for NI_MAXHOST, NI_MAXSERV
#include <poll.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>     // for read, write, close
#include <algorithm>    // for remove_if
#include "./Client.hpp"
#include "./requestMacros.hpp"

class CoreServer;
typedef void (CoreServer::*CommandHandler)(int, std::vector<std::string>&);

// Custom remove_if function for C++98
template<typename Iterator, typename Predicate>
Iterator custom_remove_if(Iterator first, Iterator last, Predicate pred) {
    Iterator result = first;
    for (; first != last; ++first) {
        if (!pred(*first)) {
            if (result != first) {
                *result = *first;
            }
            ++result;
        }
    }
    return result;
}

struct ServerData
{
    std::string         Passwd;
    int                 _socket;
    int                 _poll;
    int                 Port;
    struct sockaddr_in  ServAddr;

    ServerData() : _socket(-1), _poll(-1), Port(-1)
    {
        std::memset(&ServAddr, 0, sizeof(ServAddr));
    }
};

// Predicate for removing file descriptors
struct FdPredicate {
    int fd_to_remove;
    FdPredicate(int fd) : fd_to_remove(fd) {}
    bool operator()(const struct pollfd& pfd) const {
        return pfd.fd == fd_to_remove;
    }
};

class CoreServer
{
    private:
        ServerData                               ServData;
        std::map<int, _client>                  clients;
        std::vector<struct pollfd>              fds;
        std::vector<channel>                    channels;
        std::map<std::string, CommandHandler>   commands;
        
        void    create_socket();
        void    start_listening();
        void    start_server();
        void    cmdNick(int fd, std::vector<std::string>& args);
        void    cmdUser(int fd, std::vector<std::string>& args);
        void    cmdPass(int fd, std::vector<std::string>& args);
        void    cmdJoin(int fd, std::vector<std::string>& args);
        void    cmdPrivmsg(int fd, std::vector<std::string>& args);

    public:
        CoreServer(std::string port, std::string password);
        ~CoreServer() {} // Added destructor
        
        void    WelcomeClient();
        void    WriteEvent(int fd);
        void    handleCommands(int fd, const std::string& _cmd);
        void    ReadEvent(int fd);
        
        // Helper methods
        void    removeFd(int fd) {
            fds.erase(custom_remove_if(fds.begin(), fds.end(), FdPredicate(fd)), fds.end());
        }
};

// Non-member functions
bool IsValidPort(std::string port, int& _tport);
bool IsValidPass(std::string _pass, std::string& passwd);

#endif // CORE_SERVER_HPP
