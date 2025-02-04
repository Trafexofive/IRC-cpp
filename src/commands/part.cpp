/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 16:58:44 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/09 16:58:44 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

#include <string>

// Helper function to handle invalid channel case
static void handleInvalidChannel(Client &client, const std::string &channelName) {
  std::cout << formatServerMessage(
                   "ERROR", "PART failed: Invalid channel name " + channelName)
            << std::endl;
  client.setResponse(
      formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

// Helper function to handle successful part operation
static void handlePartSuccess(Client &client, const std::string &channelName) {
  std::string partMsg = ":" + client.getNickName() + "!" +
                        client.getFullName() + "@localhost PART " +
                        channelName + "\r\n";
  client.setResponse(partMsg);
  std::cout << formatServerMessage("SUCCESS", client.getNickName() + " left " +
                                                  channelName)
            << std::endl;
}


void CoreServer::cmdPart(int fd, std::vector<std::string>& args) {
    if (isClientDisconnected(fd))
    {
        std::cout << formatServerMessage("ERROR", "PART failed: Client is disconnected") << std::endl;
        return;
    }
    if (!isClientRegistered(fd))
    {
        std::cout << formatServerMessage("ERROR", "PART failed: Client not registered") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NOTREGISTERED, ":You have not registered"));
        return;
    }
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "PART failed: No channel specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PART :Not enough parameters"));
        return;
    }

    Client& client = clients[fd];

    // Loop on args to get the channel names
    for (std::vector<std::string>::size_type i = 1; i < args.size(); ++i) {
        std::string channelName = args[i];

        std::cout << formatServerMessage("DEBUG", client.getNickName() + " attempting to leave " + channelName) << std::endl;
        if (!isChannel(channelName)) {
            handleInvalidChannel(client, channelName);
            continue;
        }
        else if (isChannel(channelName)) {
            Channel* channel = getChannel(channelName);
            channel->removeMember(&client);
            handlePartSuccess(client, channelName);
        }
        else {
            std::cout << formatServerMessage("ERROR", "PART failed: Channel Not Found") << std::endl;
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
        }
    }
}
