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
  bool limitMode;

  bool operatorMode;
  bool keyMode;

  std::string modeString;

  CHANNEL()
      : type(UNKNOWN), inviteMode(false), topicMode(true), limitMode(false),
        operatorMode(false), keyMode(false), modeString("") {}
};

struct ClientEntry {

  enum TYPE { SUBSCRIBED, UNSUBSCRIBED };
  ClientEntry() : state(UNSUBSCRIBED), isOperator(false), client(NULL) {}

  TYPE state;

  // bool isInvited;
  bool isOperator;
  Client *client;
};

class Channel {
private:
  std::string name;
  std::string topic;
  std::string password;

  std::map<int, ClientEntry> _Registry; // yo this is shit. whats the point
                                        // chat. just use a normal map XD
  std::size_t _memberCount;
  std::size_t _memberLimit; // for mode, still needs integration with the member
                            // functions

  std::vector<Client *> _Invitees;
  std::vector<Client *> _Operators;
  CHANNEL _settings;

  Channel();

public:
  // Default constructor
  // Parameterized constructors
  Channel(const std::string &name, Client *client);
  Channel(const std::string &name, const std::string &topic, Client *client);
  Channel(const std::string &name, const std::string &topic,
          const std::string &password, Client *client);

  // Channel(const Channel &src);
  // Destructor
  ~Channel();

  // Getters
  const std::string &getName() const;
  const std::string &getTopic() const;
  const std::string &getPassword() const;

  const std::map<int, ClientEntry> &getRegistry() const { return _Registry; };

  CHANNEL::TYPE getChannelType() const { return _settings.type; }

  bool isPrivate() const { return _settings.type == CHANNEL::PRIVATE; }
  bool isPublic() const { return _settings.type == CHANNEL::PUBLIC; }
  bool isEmpty() const { return _settings.type == CHANNEL::EMPTY; }
  bool isUnknown() const { return _settings.type == CHANNEL::UNKNOWN; }

  bool isInviteMode() const { return _settings.inviteMode; }
  bool isTopicMode() const { return _settings.topicMode; }
  bool isLimitMode() const { return _settings.limitMode; }
  bool isOperatorMode() const { return _settings.operatorMode; }

  bool isKeyMode() const { return _settings.keyMode; }
  // invitee management
  bool isInvited(Client *client) {
    if (std::find(_Invitees.begin(), _Invitees.end(), client) !=
        _Invitees.end())
      return true;
    else
      return false;
  }
  void addInvitee(Client *client) { _Invitees[client->getFd()] = client; }

  // bool isInvited(Client *client)

  void setPrivate() {
    _settings.type = CHANNEL::PRIVATE;
    _settings.keyMode = true;
  }
  void setPublic() {
    _settings.type = CHANNEL::PUBLIC;
    _settings.keyMode = false;
  }
  void setEmpty() { _settings.type = CHANNEL::EMPTY; }
  void setUnknown() { _settings.type = CHANNEL::UNKNOWN; }

  // Setters
  void setName(const std::string &n);
  void setTopic(const std::string &t);
  void setPassword(const std::string &p);

  // const std::vector<Client *> &getOperators() const { return operators; }

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

  // void setSubscribed(Client *client) {
  //     for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
  //          it != _Registry.end(); ++it) {
  //       if (it->second.client == client) {
  //         it->second.state = ClientEntry::SUBSCRIBED;
  //         return;
  //       }
  //     }
  // }

  Client *getClient(const std::string &nick) {

    for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      if (it->second.client->getNickName() == nick &&
          it->second.state == ClientEntry::SUBSCRIBED)
        return it->second.client;
      else
        continue;
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

  bool isMember(Client &client) const {
    int fd = client.getFd();
    if (_Registry.find(fd) != _Registry.end() &&
        _Registry.at(fd).state == ClientEntry::SUBSCRIBED)
      return true;
    else
      return false;
  }

  bool hasPassword() const; // should be removed
  bool validatePassword(const std::string &pass) const;
  // Member management methods

  void addMember(Client *member);

  // operator management
  // bool isOperator(Client *member);
  bool isOperator(const std::string &nick);
  void addOperator(Client *member);
  void removeOperator(Client *member);

  // clean up methods
  void clearMembers();
  // Registry management methods
  void CleanRegistry();

  ClientEntry *getEntry(const std::string &nick);
  // ClientEntry &getEntry(Client *client);
  // General methods
  void broadcast(const std::string &message);
  void broadcastException(const std::string &message, Client *client);

  int getMemberCount() const;
  // Registry and state management.

  void updateModeString() {
    std::stringstream ss;

    ss << "+";

    if (_settings.inviteMode)
      ss << "i";
    if (_settings.topicMode)
      ss << "t";
    if (_settings.operatorMode)
      ss << "o";
    if (_settings.keyMode)
      ss << "k";
    if (_settings.limitMode)
      ss << "l";

    if (ss.str().size() == 1)
      _settings.modeString = "";
    else
      _settings.modeString = ss.str();
  }

  const std::string &getMode() {
    updateModeString();
    return _settings.modeString;
  }

  void setTopicMode(bool mode) { _settings.topicMode = mode; }
  void setInviteMode(bool mode) { _settings.inviteMode = mode; }
  void setOperatorMode(bool mode) { _settings.operatorMode = mode; }
  void setKeyMode(bool mode) { _settings.keyMode = mode; }
  void setLimitMode(bool mode) { _settings.limitMode = mode; }

  bool getTopicMode() const { return _settings.topicMode; }
  bool getInviteMode() const { return _settings.inviteMode; }
  bool getOperatorMode() const { return _settings.operatorMode; }
  bool getKeyMode() const { return _settings.keyMode; }
  bool getLimitMode() const { return _settings.limitMode; }

  std::size_t getMemberLimit() const { return _memberLimit; }
  void setMemberLimit(std::size_t limit) { _memberLimit = limit; }
};

#endif // CHANNEL_HPP
