/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 18:05:20 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/02 15:59:34 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

// Standard library includes
#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>

// System includes
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

// Project includes
#include "./Helpers.hpp"
#include "./Channel.hpp"
#include "./Client.hpp"
#include "./Utils.hpp"
#include "./requestMacros.hpp"
#include "./ircResponses.hpp"

// Forward declarations
class CoreServer;

typedef void (CoreServer::*CommandHandler)(int, std::vector<std::string>&);

// Custom remove_if function for C++98
template <typename Iterator, typename Predicate>
Iterator custom_remove_if(Iterator first, Iterator last, Predicate pred) {
    Iterator result = first;
    for (; first != last; ++first) {
        if (!pred(*first)) {
            if (result != first)
                *result = *first;
            ++result;
        }
    }
    return result;
}


// Predicate for removing file descriptors
struct FdPredicate {
    int fd_to_remove;
    explicit FdPredicate(int fd) : fd_to_remove(fd) {}
    bool operator()(const struct pollfd& pfd) const {
        return pfd.fd == fd_to_remove;
    }
};

class CoreServer {
private:
    ServerData ServData;
    std::map<int, Client> clients;
    std::vector<struct pollfd> fds;
    std::vector<Channel> channels;
    std::map<std::string, CommandHandler> commands;

    // Socket and server initialization
    void create_socket();
    void start_listening();
    void start_server();


    // Helper methods
    void disconnectClient(int fd);
    void broadcastToChannel(const std::string& channelName,
                          const std::string& message,
                          const std::string& except_nick = "");
    LEVEL   purgeAllClients();

    // Command handlers
    void handleCommand(int fd, const std::string& line);
    void cmdNick(int fd, std::vector<std::string>& args);
    void cmdUser(int fd, std::vector<std::string>& args);
    void cmdPass(int fd, std::vector<std::string>& args);
    void cmdJoin(int fd, std::vector<std::string>& args);
    void cmdPrivmsg(int fd, std::vector<std::string>& args);
    void cmdPart(int fd, std::vector<std::string>& args);
    void cmdPing(int fd, std::vector<std::string>& args);
    // void cmdList(int fd, std::vector<std::string>& args);


public:
    // Constructor and destructor
    CoreServer(std::string port, std::string password);
    ~CoreServer() {
        for (std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
            if (it->fd >= 0)
                close(it->fd);
        }
    }

    // Client handling methods
    void WelcomeClient();
    void WriteEvent(int fd);
    void ReadEvent(int fd);
    void handleCommands(int fd, const std::string& _cmd);

    // Utility methods
    void removeFd(int fd) {
        fds.erase(custom_remove_if(fds.begin(), fds.end(), FdPredicate(fd)), fds.end());
    }

    // Getters
    const std::map<int, Client>& getClients() const { return clients; }
    const std::vector<Channel>& getChannels() const { return channels; }

    // channel methods
    LEVEL addChannel(const std::string& name, const std::string& topic, const std::string& password);
    LEVEL removeChannel(const std::string& name);

};

// Non-member functions for validation
bool IsValidPort(std::string port, int& _tport);
bool IsValidPass(std::string _pass, std::string& passwd);

#endif // CORE_SERVER_HPP
