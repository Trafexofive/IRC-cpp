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

  Channel *channel = getChannel(name);
  if (channel == NULL) {
    printServerMessage("ERROR", "Channel " + name + " does not exist");
    return;
  }
  channel->setChannelType(CHANNEL::EMPTY);
  // channel->clear();
  // channel->CleanRegistry()
  _serverStats.totalChannels--;
}

void CoreServer::purgeEmptyChannels() {

  for (std::map<std::string, Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (it->second.getChannelType() == CHANNEL::EMPTY) {
      printServerMessage("INFO",
                         "Removing empty channel " + it->second.getName());
      channels.erase(it);
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
    printServerMessage("INFO", "REGISTERED USERS: " +
                                   numberToString(_serverStats.totalClients));
    printLine();
    return;
  }

  printLine();

  const int nameWidth = 20;
  const int countWidth = 10;
  const int typeWidth = 10;

  std::ostringstream header;
  header << std::left << std::setw(nameWidth) << "CHANNEL NAME"
         << std::setw(countWidth) << "MEMBERS" << std::setw(typeWidth)
         << "TYPE";
  printServerMessage("INFO", header.str());
  printLine();

  for (std::map<std::string, Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {

    std::ostringstream oss;

    oss << std::left << std::setw(nameWidth) << it->second.getName()
        << std::setw(countWidth) << it->second.getMemberCount()
        << std::setw(typeWidth) << it->second.getChannelType();
    printServerMessage("INFO", oss.str());
  }

  printLine();
  printServerMessage("INFO", "REGISTERED USERS: " +
                                 numberToString(_serverStats.totalClients));
  printLine();
}
