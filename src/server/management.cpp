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

  for (std::vector<Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (it->getName() == name) {
      it->setChannelType(CHANNEL::EMPTY);
      return;
    }
  }
}


void CoreServer::purgeEmptyChannels() {
    std::vector<Channel>::iterator it = channels.begin();
    
    while (it != channels.end()) {
        if (it->getChannelType() == CHANNEL::EMPTY ||
            it->getMemberCount() == 0) {
            std::cout << formatServerMessage("INFO",
                                          "Purging channel name:" + it->getName())
                     << std::endl;
            it = channels.erase(it);  // erase() returns iterator to next element
        } else {
            ++it;
        }
    }
}

void CoreServer::watchdog() {

  purgeDisconnectedClients();

  purgeEmptyChannels();
}

// void Coreserver::

// void CoreServer::removeChannel (const Channel& channel)
// {
//
// }

// Helper function to handle invalid channel case
void handleInvalidChannel(Client &client, const std::string &channelName) {
  std::cout << formatServerMessage(
                   "ERROR", "PART failed: Invalid channel name " + channelName)
            << std::endl;
  client.setResponse(
      formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

// Helper function to handle successful part operation
void handlePartSuccess(Client &client, const std::string &channelName) {
  std::string partMsg = ":" + client.getNickName() + "!" +
                        client.getFullName() + "@localhost PART " +
                        channelName + "\r\n";
  client.setResponse(partMsg);
  std::cout << formatServerMessage("SUCCESS", client.getNickName() + " left " +
                                                  channelName)
            << std::endl;
}

void CoreServer::displayChannelTable() {
  if (channels.empty()) {
    std::cout << formatServerMessage("INFO", "+ No channels available")
              << std::endl;
    std::cout << formatServerMessage(
                     "INFO",
                     "+-------------------------------------------------------")
              << std::endl;
    return;
  }
  std::cout << formatServerMessage(
                   "INFO",
                   "+-Channel Table-----------------------------------------")
            << std::endl;
  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;
  std::cout << formatServerMessage("INFO", "+ Name\t\tOnline\t\tType")
            << std::endl;
  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;

  for (std::vector<Channel>::const_iterator it = channels.begin();
       it != channels.end(); ++it) {
    std::ostringstream row;
    std::string type;
    if (it->getChannelType() == CHANNEL::PRIVATE) {
      row << "+ " << it->getName() << "\t\t" << it->getMemberCount() << "\t\t"
          << "PRIVATE";
    } else {
      row << "+ " << it->getName() << "\t\t" << it->getMemberCount() << "\t\t"
          << "PUBLIC";
    }
    row << "+ " << it->getName() << "\t\t" << it->getMemberCount() << "\t\t"
        << type;
    std::cout << formatServerMessage("INFO", row.str()) << std::endl;
  }

  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;
}
