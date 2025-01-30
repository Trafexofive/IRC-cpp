/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:13:49 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/30 11:13:49 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <algorithm>
#include <iostream>

/* ************************************************************************** */
/*                       CONSTRUCTION                                         */
/* ************************************************************************** */

// Parameterized constructors
Channel::Channel(const std::string &name, Client *client)
    : name(name), topic(""), password("") {
  std::ostringstream debug;
  debug << "Creating new channel: " << name;
  std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

  _settings.type = CHANNEL::PUBLIC;
  addMember(client);
}

Channel::Channel(const std::string &name, const std::string &topic,
                 Client *client)
    : name(name), topic(topic), password("") {

  std::ostringstream debug;
  debug << "Creating new channel: " << name << " with topic: " << topic;
  std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

  _settings.type = CHANNEL::PUBLIC;
  addMember(client);
}

Channel::Channel(const std::string &name, const std::string &topic,
                 const std::string &password, Client *client)
    : name(name), topic(topic), password(password) {
  std::ostringstream debug;
  debug << "Creating new channel: " << name << " with topic: " << topic
        << " and password";
  std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

  _settings.type = CHANNEL::PRIVATE;
  addMember(client);
}

void Channel::CleanRegistry() {
  for (std::list<ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->state == ClientEntry::UNSUBSCRIBED) {
      _Registry.erase(it);
    }
  }
}

// Destructor
Channel::~Channel() {
  std::cout << formatServerMessage("DEBUG", "Destroying channel: " + name)
            << std::endl;
  CleanRegistry();
}

/* ************************************************************************** */
/*                       GETTERS && SETTERS                                   */
/* ************************************************************************** */

// Getters
const std::string &Channel::getName() const { return name; }
const std::string &Channel::getTopic() const { return topic; }
const std::string &Channel::getPassword() const { return password; }

// Setters
void Channel::setName(const std::string &n) { name = n; }
void Channel::setTopic(const std::string &t) { topic = t; }
void Channel::setPassword(const std::string &p) { password = p; }

/* ************************************************************************** */
/*                       MANAGEMENT                                           */
/* ************************************************************************** */

// Member management methods

void Channel::addMember(Client *client) {
  for (std::list<ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->client == client)
      return;
  }

  ClientEntry entry;
  entry.client = client;
  entry.state = ClientEntry::SUBSCRIBED;
  _Registry.push_back(entry);
}

void Channel::removeMember(Client *client) {
  for (std::list<ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->client == client) {
      it->state = ClientEntry::UNSUBSCRIBED;
      return;
    }
  }
}

void Channel::removeMember(const std::string &nick) {
  for (std::list<ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->client->getNickName() == nick) {
      it->state = ClientEntry::UNSUBSCRIBED;
      return;
    }
  }
}

int Channel::getMemberCount() const {
  int count = 0;

  for (std::list<ClientEntry>::const_iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->state == ClientEntry::SUBSCRIBED)
      count++;
  }

  return count;
}

/* ************************************************************************** */
/*                       STATES/TYPES                                         */
/* ************************************************************************** */

Channel *CoreServer::getChannel(const std::string &name) {

    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end();
         ++it) {
        if (it->getName() == name) {
        return &(*it);
        }
    }
    return NULL;
}

/* ************************************************************************** */
/*                       SECTION/FUNCTION/NAME                                */
/* ************************************************************************** */

void Channel::clearMembers() {

  std::ostringstream info;
  info << "UNSUBSCRIBING all members from channel " << name;
  std::cout << formatServerMessage("INFO", info.str()) << std::endl;

  // set all members to unsubscribed
  massSetClientState(ClientEntry::UNSUBSCRIBED);
}

/* ************************************************************************** */
/*                       UTILITY                                              */
/* ************************************************************************** */

bool Channel::hasPassword() const { return !password.empty(); }

bool Channel::validatePassword(const std::string &pass) const {
  return password == pass;
}

void Channel::broadcast(const std::string &message) {

  for (std::list<ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->state == ClientEntry::SUBSCRIBED) {
      it->client->setResponse(message);
    }
  }
}
