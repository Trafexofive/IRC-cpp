/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 05:38:01 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/20 05:38:01 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../inc/Server.hpp"

Channel& getChannel(const std::string& name, std::vector<Channel>& channels) {
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == name)
            return *it;
    }
    std::cout << formatServerMessage("ERROR", "Channel " + name + " not found") << std::endl;
    // loool I hate this
    throw std::runtime_error("Channel " + name + " not found");
}


// Helper function to handle invalid channel case
void handleInvalidChannel(Client& client, const std::string& channelName) {
    std::cout << formatServerMessage("ERROR", "PART failed: Invalid channel name " + channelName) << std::endl;
    client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

// Helper function to handle successful part operation
void handlePartSuccess(Client& client, const std::string& channelName) {
    std::string partMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost PART " + channelName + "\r\n";
    client.setResponse(partMsg);
    std::cout << formatServerMessage("SUCCESS", client.getNickName() + " left " + channelName) << std::endl;
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
    row << "+ " << it->getName() << "\t\t" << it->getMembers().size() << "\t\t"
        << it->getState();
    std::cout << formatServerMessage("INFO", row.str()) << std::endl;
  }

  std::cout << formatServerMessage(
                   "INFO",
                   "+-------------------------------------------------------")
            << std::endl;
}

