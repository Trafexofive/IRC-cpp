/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */ /*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 21:33:01 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/29 21:33:01 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

#include "Client.hpp"
#include <string>
#include <vector>

struct CHANNEL {
  enum TYPE { PUBLIC, PRIVATE, EMPTY, UNKNOWN };
  TYPE type;

  bool inviteMode;
  bool topicMode;
  bool operatorMode;
  bool keyMode;
};

struct ClientEntry {

  enum TYPE { SUBSCRIBED, UNSUBSCRIBED, UNKNOWN };
  TYPE state;
  Client *client;
};

class Channel {
private:
  std::string name;
  std::string topic;
  std::string password;

  // std::vector<Client *> members;

  std::list<ClientEntry>
      _Registry; // used along side members to keep track of the clients in the
                 // channel (using states), handlers still need to change to
                 // accomomdate changes.
  // std::vector<Client> &operators;

CHANNEL _settings;

public:
  // Default constructor
  Channel();
  // Parameterized constructors
  Channel(const std::string &name, Client *client);
  Channel(const std::string &name, const std::string &topic, Client *client);
  Channel(const std::string &name, const std::string &topic,
          const std::string &password, Client *client);
  // Destructor
  ~Channel();

  // Getters
const std::string &getName() const { return name; }
const std::string &getTopic() const { return topic; }
const std::string &getPassword() const { return password; }

  const std::list<ClientEntry> &getRegistry() const { return _Registry; };

  CHANNEL::TYPE getChannelType() const { return _settings.type; }
ClientEntry::TYPE getClientState(Client *client) const {
    for (std::list<ClientEntry>::const_iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      if (it->client == client)
        return it->state;
    }
    return ClientEntry::UNKNOWN;
}

  // Setters
  void setName(const std::string &n);
  void setTopic(const std::string &t);
  void setPassword(const std::string &p);

  void setChannelType(CHANNEL::TYPE type) { _settings.type = type; }
  void setClientState(Client *client, ClientEntry::TYPE state) {
    for (std::list<ClientEntry>::iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      if (it->client == client) {
        it->state = state;
        return;
      }
    }
  }

  // Validation methods
  bool isMember(Client *client) const {
    for (std::list<ClientEntry>::const_iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      if (it->client == client && it->state == ClientEntry::SUBSCRIBED)
        return true;
    }
    return false;
  }

  bool isMember(Client &client) const {
    for (std::list<ClientEntry>::const_iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      if (it->client == &client && it->state == ClientEntry::SUBSCRIBED)
        return true;
    }
    return false;
  }

  bool hasPassword() const; // should be removed
  bool validatePassword(
      const std::string &pass) const; // should be renamed to validate password
  // Member management methods
  void removeMember(const std::string &nick);
  void removeMember(Client *obj);

  void addMember(Client *member);

// clean up methods
  void clearMembers();


  // General methods
  void broadcast(const std::string &message);

  int getMemberCount() const;
};

// helper functions

bool isChannel(const std::string &name);
Channel &getChannel(const std::string &name, std::vector<Channel> &channels);
void removeClientFromChannel(Client &client, Channel &channel);
void handleInvalidChannel(Client &client, const std::string &channelName);
void handlePartSuccess(Client &client, const std::string &channelName);
std::string getChannelsString(const std::vector<Channel> &channels);

#endif // CHANNEL_HPP
