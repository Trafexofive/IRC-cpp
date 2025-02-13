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

  printServerMessage("DEBUG", "Creating new channel: " + name);

  _settings.type = CHANNEL::PUBLIC;
  _memberCount = 0;
  setKeyMode(false);
  addMember(client);
  addOperator(client);
}

Channel::Channel(const std::string &name, const std::string &topic,
                 Client *client)
    : name(name), topic(topic), password("") {

  printServerMessage("DEBUG",
                     "Creating new channel: " + name + " with topic: " + topic);

  _settings.type = CHANNEL::PUBLIC;
  _memberCount = 0;
  setKeyMode(false);
  addMember(client);
  addOperator(client);
}

Channel::Channel(const std::string &name, const std::string &topic,
                 const std::string &password, Client *client)
    : name(name), topic(topic), password(password) {
  printServerMessage("DEBUG",
                     "Creating new channel: " + name + " with topic: " + topic);

  _settings.type = CHANNEL::PRIVATE;
  _memberCount = 0;
  setKeyMode(true);
  addMember(client);
  addOperator(client);
}

void Channel::CleanRegistry() {
  for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end();) {
    if (it->second.state == ClientEntry::UNSUBSCRIBED) {
      _Registry.erase(it++);
    } else {
      ++it;
    }
  }
}

// Destructor
Channel::~Channel() {
  printServerMessage("DEBUG", "Destroying channel: " + name);
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
  if (client->isDisconnected())
    return;
  int fd = client->getFd();
  if (_Registry[fd].client == client &&
      _Registry[fd].state == ClientEntry::SUBSCRIBED) {
    printServerMessage("WARNING",
                       "Client already added to channel. Server Omitting ...");
    return;
  } else if (_Registry[fd].client == client) {
    _Registry[fd].state = ClientEntry::SUBSCRIBED;
    _memberCount++;
    return;
  }

  // adding new client entry to the channel
  ClientEntry entry;
  entry.client = client;
  entry.state = ClientEntry::SUBSCRIBED;
  _Registry[client->getFd()] = entry;
  _memberCount++;
}

void Channel::removeMember(Client *client) {
  if (client->isDisconnected())
    return;
  int fd = client->getFd();

  if (_Registry[fd].client == client &&
      _Registry[fd].state == ClientEntry::UNSUBSCRIBED) {
    printServerMessage(
        "WARNING", "Client already removed from channel. Server Omitting ...");
    return;
  } else if (_Registry[fd].client == client) {
    _Registry[fd].state = ClientEntry::UNSUBSCRIBED;
    _memberCount--;
    if (_memberCount == 0)
      setChannelType(CHANNEL::EMPTY);
    return;
  }
}

int Channel::getMemberCount() const { return _memberCount; }

bool Channel::isOperator(const std::string &nick) {
  ClientEntry *entry = getEntry(nick);
  if (entry && entry->isOperator) {
    printServerMessage("DEBUG", "CHANNEL :" + this->getName() + "Client " + nick +
                              " is an operator");
    return true;
  }
  printServerMessage("DEBUG", "CHANNEL :" + this->getName() + "Client " + nick +
                                  " is not an operator");
  return false;
}

ClientEntry *Channel::getEntry(const std::string &nick) {
  for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->second.client->getNickName() == nick &&
        it->second.state == ClientEntry::SUBSCRIBED &&
        !it->second.client->isDisconnected())
      return &it->second;
    else
      continue;
  }
  return NULL;
}

void Channel::addOperator(Client *member) {
  if (member->isDisconnected())
    return;
  else {
    ClientEntry *entry = getEntry(member->getNickName());
    if (entry) {
      entry->isOperator = true;
      printServerMessage("DEBUG", "Client [" + member->getNickName() +
                                      "] is now an operator");

    } else {
      printServerMessage("WARNING", "addOperator: Client not found in channel");
    }
  }
}

void Channel::removeOperator(Client *member) {
  if (member->isDisconnected())
    return;
  else {
    ClientEntry *entry = getEntry(member->getNickName());
    if (entry)
      entry->isOperator = false;
    else {
      printServerMessage("WARNING",
                         "removeOperator: Client not found in channel");
    }
  }
}

/* ************************************************************************** */
/*                       STATES/TYPES                                         */
/* ************************************************************************** */

Channel *CoreServer::getChannel(const std::string &name) {

  std::map<std::string, Channel>::iterator it = channels.find(name);
  if (it == channels.end() || it->second.isEmpty()) {
    printServerMessage("WARNING", "Channel " + name + " does not exist");
    return NULL;
  }
  return &it->second;
}

/* ************************************************************************** */
/*                       SECTION/FUNCTION/NAME                                */
/* ************************************************************************** */

void Channel::clearMembers() {

  printServerMessage("DEBUG", "Clearing all members from channel: " + name);
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

  for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->second.state == ClientEntry::SUBSCRIBED) {
      it->second.client->setResponse(message);
    }
  }
}

void Channel::broadcastException(const std::string &message, Client *client) {
  printServerMessage(
      "DEBUG", "Broadcasting exception to all clients in channel: " + name);
  for (std::map<int, ClientEntry>::iterator it = _Registry.begin();
       it != _Registry.end(); ++it) {
    if (it->second.state == ClientEntry::SUBSCRIBED &&
        it->second.client != client) {
      printServerMessage("DEBUG", "Broadcasting exception to client: " +
                                      it->second.client->getTarget());
      it->second.client->setResponse(message);
    }
  }
}
