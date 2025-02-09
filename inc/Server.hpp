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
#include "./Utils.hpp"
#include "./ircResponses.hpp"

// Forward declarations
class CoreServer;

typedef void (CoreServer::*CommandHandler)(int, std::vector<std::string> &);

struct FdPredicate {
  int fd_to_remove;
  explicit FdPredicate(int fd) : fd_to_remove(fd) {}
  bool operator()(const struct pollfd &pfd) const {
    return pfd.fd == fd_to_remove;
  }
};

struct Stats {

  int totalClients;
  int totalChannels;
  int totalMessages;

  int uptime;

  int tickRate;
  int tick;

  void reset() {
    totalClients = 0;
    totalChannels = 0;
    totalMessages = 0;
    uptime = 0;
    tick = 0;
  }

  void printStats() {
    printServerMessage("INFO", "---------------------------------");
    printServerMessage("INFO",
                       "Total Clients: " + numberToString(totalClients));
    printServerMessage("INFO",
                       "Total Channels: " + numberToString(totalChannels));
    printServerMessage("INFO",
                       "Total Messages: " + numberToString(totalMessages));
    printServerMessage("INFO", "Uptime: " + numberToString(uptime));
    printServerMessage("INFO", "Tick Rate: " + numberToString(tickRate));
    printServerMessage("INFO", "Tick: " + numberToString(tick));
    printServerMessage("INFO", "---------------------------------");
  }
};

class CoreServer {
private:
  ServerData ServData;
  Stats _serverStats;

  std::map<int, Client> clients;

  std::vector<struct pollfd> fds;

  // std::vector<Channel> channels;

  std::map<std::string, Channel> channels; // in favor of quick lookup and
  // time complexity

  std::map<std::string, CommandHandler> commands;

  // TickRate methods

  void UpdateUptime() {
    if (_serverStats.uptime == 0)
      _serverStats.uptime = time(0);
    time_t now = time(NULL);
    _serverStats.uptime = now - _serverStats.uptime;
  }

  // void UpdateServerStats()
  // {
  //
  // }
  void TickCycle() {
    // we can give up on this for now
    if (_serverStats.tick == _serverStats.tickRate) {
      printServerMessage("SYSTEM", "Tick Cycle Initiated");
      printServerMessage("SYSTEM", "Cleaning up Server ...");
      // Execute state-based operations
      // CheckClientTimeouts();
      // CleanEmptyChannels();
      // UpdateServerStats();
      purgeDisconnectedClients();

      purgeEmptyChannels();
      UpdateUptime();
      _serverStats.tick = 0;
    } else
      _serverStats.tick++;
  }

  // Socket and server initialization
  void create_socket();
  void start_listening();
  void start_server();

  // Cleaner methods
  void disconnectClient(int fd);
  void handleDisconnect(int fd);
  void sendNotice(int fd, const std::string &message);

  void purgeEmptyChannels();
  void purgeDisconnectedClients();

  // Client management methods
  void disableClient(int fd);
  void disableChannel(const std::string &name);

  // Smp Client State Getters
  bool isClientRegistered(int fd) { return clients[fd].isRegistered(); }
  bool isClientAuthenticated(int fd) { return clients[fd].isAuthenticated(); }
  bool isClientDisconnected(int fd) { return clients[fd].isDisconnected(); }

  // Command handlers
  void handleCommand(int fd, const std::string &line);

  // Actual commands
  void cmdNick(int fd, std::vector<std::string> &args);
  void cmdList(int fd, std::vector<std::string> &args);
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
  void cmdTopic(int fd, std::vector<std::string> &args);
  void cmdInvite(int fd, std::vector<std::string> &args);
  void cmdKick(int fd, std::vector<std::string> &args);

  // channel management methods
  void kickUserFromChannel(int fd, const std::string &target,
                           const std::string &reason);
  void joinChannel(Client &client, const std::string &channelName);
  void joinChannel(Client &client, const std::string &channelName,
                   const std::string &key);

  bool isChannel(const std::string &name);
  void unsubFromChannels(int fd) {
    if (isClientDisconnected(fd))
      return;
    for (std::map<std::string, Channel>::iterator it = channels.begin();
         it != channels.end(); ++it) {
      if (it->second.isEmpty())
        continue;
      if (it->second.isMember(clients[fd])) {
        printServerMessage("INFO", "Removing client from channel " +
                                       it->second.getName());
        it->second.removeMember(&clients[fd]);
      }
    }
  }
  // privmsg helpers client && channel
  void send_message_to_channel(int fd, const std::string &channel,
                               const std::string &message);
  void send_message_to_user(int fd, const std::string &target,
                            const std::string &message);

  // Events / Client handlers
  void WelcomeClient();
  void WriteEvent(int fd);
  void ReadEvent(int fd);

  // Display / Debug
  void displayChannelTable();

public:
  // Constructor and destructor
  CoreServer(std::string port, std::string password);

  // Utility methods

  Channel *getChannel(const std::string &name);

  ~CoreServer() {

    for (std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end();
         ++it) {
      if (it->fd >= 0)
        close(it->fd);
    }
    channels.clear();
    clients.clear();
    _serverStats.printStats();
  }

  void removeChannel(const Channel &channel) {
    for (std::map<std::string, Channel>::iterator it = channels.begin();
         it != channels.end(); ++it) {
      if (it->second.getName() == channel.getName()) {
        channels.erase(it);
        printServerMessage("INFO", "Channel " + channel.getName() + " removed");
        break;
      }
    }
  }

  bool validatePassword(const std::string &password) {
    return ServData.Passwd == password;
  }

  // void start() {
  //     start_listening();
  //     start_server();
  // }

  void broadcast(Channel &channel, const std::string &message) {

    for (std::map<int, ClientEntry>::const_iterator it =
             channel.getRegistry().begin();
         it != channel.getRegistry().end(); ++it) {

      if (it->second.state == ClientEntry::SUBSCRIBED) {
        it->second.client->setResponse(message);
        this->WriteEvent(it->first);
      }
    }
  }

  void broadcastException(Channel &channel, const std::string &message,
                          Client *client) {
    printServerMessage("DEBUG",
                       "Broadcasting exception to all clients in channel: " +
                           channel.getName());
    for (std::map<int, ClientEntry>::const_iterator it =
             channel.getRegistry().begin();
         it != channel.getRegistry().end(); ++it) {
      if (it->second.state == ClientEntry::SUBSCRIBED &&
          it->second.client != client) {
        printServerMessage("DEBUG", "Broadcasting exception to client: " +
                                        it->second.client->getTarget());
        it->second.client->setResponse(message);
        this->WriteEvent(it->first);
      }
    }
  }

  void broadcastChannels(const std::string &message) {
    for (std::map<std::string, Channel>::iterator it = channels.begin();
         it != channels.end(); ++it) {
      broadcast(it->second, message);
    }
  }

void broadcastChannelsException(const std::string &message, Client *client) {
    for (std::map<std::string, Channel>::iterator it = channels.begin();
         it != channels.end(); ++it) {
      broadcastException(it->second, message, client);
    }
}

void broadcastSubbedChannels(const std::string &message, Client *client) {
    for (std::map<std::string, Channel>::iterator it = channels.begin();
         it != channels.end(); ++it) {
      if (it->second.isMember(*client)) {
        broadcast(it->second, message);
      }
    }
  }

  // void broadcastException(const std::string &message, Client *client) {
  //     printServerMessage("DEBUG", "Broadcasting exception to all clients in
  //     channel: " + name);
  //   for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
  //        it != _Registry.end(); ++it) {
  //     if (it->second.state == ClientEntry::SUBSCRIBED && it->second.client !=
  //     client) {
  //         printServerMessage("DEBUG", "Broadcasting exception to client: " +
  //         it->second.client->getTarget());
  //       it->second.client->setResponse(message);
  //     }
  //   }
  // }

  void createChannel(const std::string &name, const std::string &topic,
                     const std::string &password, Client *client) {
    channels.insert(std::pair<std::string, Channel>(
        name, Channel(name, topic, password, client)));
    _serverStats.totalChannels++;
  }

  void createChannel(const std::string &name, Client *client) {
    channels.insert(
        std::pair<std::string, Channel>(name, Channel(name, client)));
    _serverStats.totalChannels++;
  }
  void removeChannel(const std::string &name) {
    channels.erase(name);
    _serverStats.totalChannels--;
  }
};

// Non-member functions for validation
bool IsValidPort(std::string port, int &_tport);
bool IsValidPass(std::string _pass, std::string &passwd);

#endif // CORE_SERVER_HPP
