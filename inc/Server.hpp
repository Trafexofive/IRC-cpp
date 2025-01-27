/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 18:05:20 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/26 21:51:17 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

// Standard library includes
#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
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
#include "./Channel.hpp"
#include "./Client.hpp"
#include "./Helpers.hpp"
#include "./Utils.hpp"
#include "./ircResponses.hpp"
#include "./requestMacros.hpp"

// Forward declarations
class CoreServer;

typedef void (CoreServer::*CommandHandler)(int, std::vector<std::string> &);

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
  bool operator()(const struct pollfd &pfd) const {
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
  LEVEL purgeAllClients();

  // Command handlers
  void handleCommand(int fd, const std::string &line);

  void cmdNick(int fd, std::vector<std::string> &args);
  void cmdUser(int fd, std::vector<std::string> &args);
  void cmdPass(int fd, std::vector<std::string> &args);
  void cmdJoin(int fd, std::vector<std::string> &args);
  void cmdPrivmsg(int fd, std::vector<std::string> &args);
  void cmdPart(int fd, std::vector<std::string> &args);
  void cmdPing(int fd, std::vector<std::string> &args);
  void cmdCap(int fd, std::vector<std::string> &args);
  void cmdQuit(int fd, std::vector<std::string> &args);
  void cmdMode(int fd, std::vector<std::string> &args);
  void cmdWho(int fd, std::vector<std::string> &args);
  // void cmdList(int fd, std::vector<std::string>& args);

public:
  // Constructor and destructor
  CoreServer(std::string port, std::string password);
  ~CoreServer() {
    for (std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end();
         ++it) {
      if (it->fd >= 0)
        close(it->fd);
    }
  }

  // Client handling methods
  void WelcomeClient();
  void WriteEvent(int fd);
  void ReadEvent(int fd);

  // Utility methods

  bool isChannel(const std::string &name);

  // Getters
  const std::map<int, Client> &getClients() const { return clients; }
  const std::vector<Channel> &getChannels() const { return channels; }

  // channel methods
  void joinChannel(Client &client, const std::string &channelName);
  void joinChannel(Client &client, const std::string &channelName,
                   const std::string &key);

  void addChannel(const std::string &name, const std::string &topic,
                  const std::string &password);
  void removeChannel(const std::string &name);
  // void joinSingleChannel(Client &client, const std::string &channelName,
  // const std::string &key);
  void joinSingleChannel(Client &client, const std::string &channelName);
  void broadcastToChannel(const std::string &channelName,
                          const std::string &message,
                          const std::string &except_nick = "");

  void leaveAllChannels(const Client &client);

    void channelDestroyer();

    void displayChannelTable() ;
    void channelStatusHandler();
};

// Non-member functions for validation
bool IsValidPort(std::string port, int &_tport);
bool IsValidPass(std::string _pass, std::string &passwd);

#endif // CORE_SERVER_HPP
