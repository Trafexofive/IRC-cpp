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


/* ************************************************************************** */
/*                       CONSTRUCTION                                         */
/* ************************************************************************** */

// Parameterized constructors
Channel::Channel(const std::string &name, Client *client)
    : name(name), topic(""), password("") {
  std::ostringstream debug;
  debug << "Creating new channel: " << name;
  std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

  _type.state = CHANNEL::PUBLIC;
  members.push_back(client);
}

Channel::Channel(const std::string &name, const std::string &topic,
                 Client *client)
    : name(name), topic(topic), password("") {

  std::ostringstream debug;
  debug << "Creating new channel: " << name << " with topic: " << topic;
  std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

  _type.state = CHANNEL::PUBLIC;
  members.push_back(client);
}

Channel::Channel(const std::string &name, const std::string &topic,
                 const std::string &password, Client *client)
    : name(name), topic(topic), password(password) {
  std::ostringstream debug;
  debug << "Creating new channel: " << name << " with topic: " << topic
        << " and password";
  std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

  _type.state = CHANNEL::PRIVATE;
  members.push_back(client);
}

// Destructor
Channel::~Channel() {
  std::cout << formatServerMessage("DEBUG", "Destroying channel: " + name)
            << std::endl;
  // members.clear();
}

/* ************************************************************************** */
/*                       GETTERS && SETTERS                                   */
/* ************************************************************************** */

// Getters
const std::string &Channel::getName() const { return name; }
const std::string &Channel::getTopic() const { return topic; }
const std::string &Channel::getPassword() const { return password; }
const std::vector<Client *> &Channel::getMembers() const { return members; }

std::string getChannelsString(const std::vector<Channel> &channels) {
  std::string channelsStr;
  channelsStr.clear();
  for (size_t i = 0; i < channels.size(); ++i) {
    channelsStr += channels[i].getName();
    if (i < channels.size() - 1) {
      channelsStr += " ";
    }
  }
  return channelsStr;
}

// Setters
void Channel::setName(const std::string &n) { name = n; }
void Channel::setTopic(const std::string &t) { topic = t; }
void Channel::setPassword(const std::string &p) { password = p; }

/* ************************************************************************** */
/*                       MANAGEMENT                                           */
/* ************************************************************************** */

// Member management methods
void Channel::addMember(Client *member) {
  for (std::vector<Client *>::const_iterator it = members.begin();
       it != members.end(); ++it) {
    if ((*it)->getNickName() == member->getNickName()) {
      std::ostringstream warning;
      warning << "User " << member->getNickName()
              << " is already in channel. Ignoring..." << this->name;
      std::cout << formatServerMessage("WARNING", warning.str()) << std::endl;
      return;
    }
  }
  members.push_back(member);
  std::ostringstream success;
  success << "Added " << member->getNickName() << " to channel " << name
          << " (Total members: " << members.size() << ")";
  std::cout << formatServerMessage("INFO", success.str()) << std::endl;
}

bool Channel::removeMember(const std::string &nickname) {
  for (std::vector<Client *>::iterator it = members.begin();
       it != members.end(); ++it) {
    if ((*it)->getNickName() == nickname) {
      members.erase(it);
      std::ostringstream success;
      success << "Removed " << nickname << " from channel " << name
              << " (Remaining members: " << members.size() << ")";
      std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
      // if (!members.size()) {
      //     this->setState("EMPTY");
      // }
      return true;
    }
  }
  std::cout << formatServerMessage("WARNING", "Client Instance Not Found.")
            << std::endl;
  return false;
}

bool Channel::removeMember(Client *client) {
  for (std::vector<Client *>::iterator it = members.begin();
       it != members.end(); ++it) {

    if ((*it)->getNickName() == client->getNickName()) {
      members.erase(it);
      std::ostringstream success;
      success << "Removed " << client->getNickName() << " from channel " << name
              << " (Remaining members: " << members.size() << ")";
      std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
      // if (members.empty()) {
      //     this->setState("EMPTY");
      // }
      return true;
    }
  }
  std::cout << formatServerMessage("WARNING", "Client Instance Not Found.")
            << std::endl;
  return false;
}

int Channel::getMemberCount() const { 
    int count = 0;
    if (this->getMembers().empty()) 
        return count;
    for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if ((*it)->getStatus() == "DISCONNECTED") {
            continue;
        } else {
            count++;
        }
    }
    return count;
}

/* ************************************************************************** */
/*                       STATES/TYPES                                         */
/* ************************************************************************** */



/* ************************************************************************** */
/*                       SECTION/FUNCTION/NAME                                */
/* ************************************************************************** */

// std::string Channel::getMembersList() const {
//     std::ostringstream memberList;
//
//     for (std::vector<Client*>::const_iterator it = members.begin(); it !=
//     members.end(); ++it) {
//         if (it != members.begin())
//             memberList << " ";
//         memberList << (*it)->getNickName();
//     }
//     return memberList.str();
// }

void Channel::clearMembers() {
  members.clear();
  std::ostringstream info;
  info << "Cleared all members from channel " << name;
  std::cout << formatServerMessage("INFO", info.str()) << std::endl;
}

/* ************************************************************************** */
/*                       UTILITY                                              */
/* ************************************************************************** */

bool Channel::hasPassword() const { return !password.empty(); }

bool Channel::checkPassword(const std::string &pass) const {
  return password == pass;
}

void Channel::broadcast(const std::string &message) {
  for (std::vector<Client *>::iterator it = members.begin();
       it != members.end(); ++it) {
    Client *client = *it;
    client->setResponse(message);
  }
}
