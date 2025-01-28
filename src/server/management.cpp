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

void CoreServer::purgeDisconnectedClients() {
    for (std::map <int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getStatus() == "DISCONNECTED") {
            std::cout << formatServerMessage("INFO", "Purging client " + it->second.getNickName()) << std::endl;

            leaveAllChannels(it->second);

            clients.erase(it);
        }
    }
}

void CoreServer::purgeEmptyChannels() {
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getMemberCount() == 0){
            std::cout << formatServerMessage("INFO", "Purging empty channel " + it->getName()) << std::endl;
            channels.erase(it);
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

void CoreServer::leaveAllChannels(const Client &client) {
  if (channels.empty()) {
    std::cout
        << formatServerMessage(
               "WARNING",
               "No channels available, return from leaveAllChannels Scope")
        << std::endl;
    return;
  }

  for (std::vector<Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (it->isMember(client))
      it->removeMember(client);
  }
}

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
    row << "+ " << it->getName() << "\t\t" << it->getMemberCount() << "\t\t"
        << it->getState();
    std::cout << formatServerMessage("INFO", row.str()) << std::endl;
  }

  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;
}
