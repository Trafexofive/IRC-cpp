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
};

struct ClientEntry {

  enum TYPE { SUBSCRIBED, UNSUBSCRIBED };

  TYPE state;

  Client *client;
};

class Channel {
private:
  std::string name;
  std::string topic;
  std::string password;

  std::vector<Client *> operators; // still not sure about handling this this way.

  std::list<ClientEntry> _Registry;
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

  const std::list<ClientEntry> &getRegistry() const { return _Registry; };

  CHANNEL::TYPE getChannelType() const { return _settings.type; }

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

  void massSetClientState(ClientEntry::TYPE state) {
    for (std::list<ClientEntry>::iterator it = _Registry.begin();
         it != _Registry.end(); ++it) {
      it->state = state;
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
  bool validatePassword(const std::string &pass) const;
  // Member management methods
  void removeMember(const std::string &nick);
  void removeMember(Client *obj);

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

  int getMemberCount() const;
  // Registry and state management.
};

#endif // CHANNEL_HPP
