/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   management.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/25 19:30:25 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/25 19:30:25 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <iostream>
#include <ostream>
#include <vector>

// for all the server methodes that manage clients and channels.

void CoreServer::disableClient(int fd) {

  Client &client = clients[fd];

  client.setStatus(STATUS::DISCONNECTED);
  // client.clear();

  close(fd);
  client.setFd(-1);
}

void CoreServer::disableChannel(const std::string &name) {

    // in favor of quick lookup and time complexity <std::string,Channel>
  for (std::vector<Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (it->getName() == name) {
      it->setChannelType(CHANNEL::EMPTY);
    _serverStats.totalChannels--; //setChannelType should handle this.
      return;
    }
  }
}

void CoreServer::purgeEmptyChannels() {
  std::vector<Channel>::iterator it = channels.begin();

  while (it != channels.end()) {
    if (it->getChannelType() == CHANNEL::EMPTY || it->getMemberCount() == 0) {
        printServerMessage("INFO", "Purging empty channel: " + it->getName());
        _serverStats.totalChannels--;
      it = channels.erase(it); // erase() returns iterator to next element
    } else {
      ++it;
    }
  }
}

/* ************************************************************************** */
/*                       CLIENT MANAGEMENT                                    */
/* ************************************************************************** */

void CoreServer::sendNotice(int fd, const std::string &message) {

  clients[fd].setResponse(formatResponse(NOTICE, message));
}

/* ************************************************************************** */
/*                       DISPLAY                                              */
/* ************************************************************************** */

static void printLine() {
  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;
}

void CoreServer::displayChannelTable() {
  if (channels.empty()) {
    printLine();
    printServerMessage("INFO", "No channels available");
    printLine();
    printServerMessage("INFO", "REGISTERED USERS: " + numberToString(_serverStats.totalClients));
    printLine();
    return;
  }

  printLine();

  const int nameWidth = 20;
  const int countWidth = 10;
  const int typeWidth = 10;

  std::ostringstream header;
  header << std::left << std::setw(nameWidth) << "CHANNEL NAME"
         << std::setw(countWidth) << "MEMBERS"
         << std::setw(typeWidth) << "TYPE";
  printServerMessage("INFO", header.str());
  printLine();

  for (std::vector<Channel>::const_iterator it = channels.begin();
       it != channels.end(); ++it) {
    std::ostringstream row;
    std::string type;

    if (it->getChannelType() == CHANNEL::PRIVATE) {
      type = "PRIVATE";
    } else if (it->getChannelType() == CHANNEL::PUBLIC) {
      type = "PUBLIC";
    } else if (it->getChannelType() == CHANNEL::EMPTY) {
      type = "EMPTY";
    } else if (it->getChannelType() == CHANNEL::UNKNOWN) {
      type = "UNKNOWN";
    }

    row << std::left << std::setw(nameWidth) << it->getName()
        << std::setw(countWidth) << it->getMemberCount()
        << std::setw(typeWidth) << type;
    printServerMessage("INFO", row.str());
  }

  printLine();
  printServerMessage("INFO", "REGISTERED USERS: " + numberToString(_serverStats.totalClients));
  printLine();
}
