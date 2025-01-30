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

// for all the server methodes that manage clients and channels.

void CoreServer::disableClient(int fd) {

    Client& client = clients[fd];

    client.isStatus(STATUS::DISCONNECTED);
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
    if (channels.empty()) {
        std::cout << formatServerMessage("WARNING", "No channels available, return from purgeEmptyChannels Scope") << std::endl;
        return;
    }
    int i = 0;
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        std::cout << formatServerMessage("DEBUG", "Checking channel index") << std::endl;
        if (it->getMemberCount() == 0 || it->getChannelType() == CHANNEL::EMPTY) {
            std::cout << formatServerMessage("INFO", "Purging empty channel " + it->getName()) << std::endl;

            channels.erase(it);
            i++;
        }
    }
}

void CoreServer::watchdog() {

  if (channels.empty()) {
    std::cout
        << formatServerMessage(
               "WARNING",
               "No channels available, return from channelStatusHandler Scope")
        << std::endl;
    return;
  }

  purgeEmptyChannels();

  purgeDisconnectedClients();
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
    std::string type = it->getChannelType() == CHANNEL::PUBLIC ? "PUBLIC" : "PRIVATE";
    row << "+ " << it->getName() << "\t\t" << it->getMemberCount() << "\t\t"
        << type;
    std::cout << formatServerMessage("INFO", row.str()) << std::endl;
  }

  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;
}
