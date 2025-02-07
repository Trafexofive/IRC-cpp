/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:00:13 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/30 18:00:13 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Utils.hpp"
#include "ircResponses.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

struct CHANNEL {
  enum TYPE { PUBLIC, PRIVATE, EMPTY, UNKNOWN };

  TYPE type;

  bool inviteMode;
  bool topicMode;
  bool operatorMode;
  bool keyMode;

  std::string key;
  std::string modeString;

};

struct ClientEntry {

  enum TYPE { SUBSCRIBED, UNSUBSCRIBED };

  TYPE state;

  bool isOperator;
  Client *client;

};

class Channel {
private:
  std::string name;
  std::string topic;
  std::string password;

  std::vector<Client *> 
      operators; // still not sure about handling this this way.

  // std::list<ClientEntry> _Registry;
  std::map<int, ClientEntry> _Registry;
  int _memberCount;

  CHANNEL _settings;

  Channel();

public:
  // Default constructor
  // Parameterized constructors
  Channel(const std::string &name, Client *client);
  Channel(const std::string &name, const std::string &topic, Client *client);
  Channel(const std::string &name, const std::string &topic,
          const std::string &password, Client *client);
  // Destructor
  ~Channel();

  // Getters
  const std::string &getName() const;
  const std::string &getTopic() const;
  const std::string &getPassword() const;

  // const std::list<ClientEntry> &getRegistry() const { return _Registry; };
  const std::map<int ,ClientEntry> &getRegistry() const { return _Registry; };

  CHANNEL::TYPE getChannelType() const { return _settings.type; }
bool isPrivate() const { return _settings.type == CHANNEL::PRIVATE; }
bool isPublic() const { return _settings.type == CHANNEL::PUBLIC; }
bool isEmpty() const { return _settings.type == CHANNEL::EMPTY; }
bool isUnknown() const { return _settings.type == CHANNEL::UNKNOWN; }

  // Setters
  void setName(const std::string &n);
  void setTopic(const std::string &t);
  void setPassword(const std::string &p);
  void setMode(const std::string &mode) {
    if (mode[0] == '+' || mode[0] == '-') {
      for (size_t i = 1; i < mode.size(); i++) {

        switch (mode[i]) {
        case 'i':
          _settings.inviteMode = mode[0] == '+';
          break;
        case 't':
          _settings.topicMode = mode[0] == '+';
          break;
        case 'o':
          _settings.operatorMode = mode[0] == '+';
          break;
        case 'k':
          _settings.keyMode = mode[0] == '+';
          break;
        default:
          break;
        }
      }
    } else {

        printServerMessage("ERROR", "Invalid mode string");
    }
  }
  const std::vector<Client *> &getOperators() const { return operators; }

  void updateModeString() {
    std::stringstream ss;
    _settings.modeString.clear();
    ss << "+";
    if (_settings.inviteMode)
      ss << "i";
    if (_settings.topicMode)
      ss << "t";
    if (_settings.operatorMode)
      ss << "o";
    if (_settings.keyMode)
      ss << "k";
    _settings.modeString = ss.str();
  }

  const std::string &getMode() {
    this->updateModeString();
    return _settings.modeString;
  }

  void setChannelType(CHANNEL::TYPE type) { _settings.type = type; }

  void setClientState(Client *client, ClientEntry::TYPE state) {
    for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      if (it->second.client == client) {
        it->second.state = state;
        return;
      }
    }
  }

Client *getClient(const std::string &nick) {
    for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
        if (it->second.client->getNickName() == nick && it->second.state == ClientEntry::UNSUBSCRIBED)
            return NULL;
    if (it->second.client->getNickName() == nick && it->second.state == ClientEntry::SUBSCRIBED)
        return it->second.client;
    }
    return NULL;
}

Client *getClient(int fd) {
    if (_Registry.find(fd) != _Registry.end())
      return _Registry.at(fd).client;
    else
      return NULL;
}

  void massSetClientState(ClientEntry::TYPE state) {
    for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      it->second.state = state;
    }
  }

  void removeMember(Client *client);
// this is a better approach to the above method O(1) instead of O(n)
  // bool isMember(Client *client) const {
  bool isMember(Client &client) const {
      // maybe use if (fd == -1) // just incase
    int fd = client.getFd();
    if (_Registry.find(fd) != _Registry.end() && _Registry.at(fd).state == ClientEntry::SUBSCRIBED)
      return true;
    else 
      return false;
  }


  bool hasPassword() const; // should be removed
  bool validatePassword(const std::string &pass) const;
  // Member management methods

  void addMember(Client *member);
  void addOperator(Client *member) {
    if (member->isDisconnected())
      return;
    // if (member->isOperator())
    //   return;
    //   handle is already an operator
    if (std::find(operators.begin(), operators.end(), member) ==
        operators.end())
      operators.push_back(member);
  }

  // clean up methods
  void clearMembers();
  void CleanRegistry();

  // General methods
  void broadcast(const std::string &message);
void broadcastException(const std::string &message, Client *client) ;

  int getMemberCount() const;
  // Registry and state management.
};

#endif // CHANNEL_HPP
