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

static void handleInvalidChannel(Client &client, const std::string &channelName) {
    printServerMessage("ERROR", "PART failed: Invalid channel");
    client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
}

static void handlePartSuccess(Client &client, const std::string &channelName) {
    printServerMessage("INFO", client.getNickName() + " has left " + channelName);
    client.setResponse(":" + client.getNickName() + " PART " + channelName + "\r\n");
}

void CoreServer::cmdPart(int fd, std::vector<std::string>& args) {
    if (isClientDisconnected(fd))
    {
        printServerMessage("ERROR", "PART failed: Client is disconnected");
        return;
    }
    if (!isClientRegistered(fd))
    {
        printServerMessage("ERROR", "PART failed: No client registered");
        clients[fd].setResponse(formatResponse(ERR_NOTREGISTERED, ":You have not registered"));
        return;
    }
    if (args.size() < 2) {
        printServerMessage("ERROR", "PART failed: Not enough parameters");
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PART :Not enough parameters"));
        return;
    }

    Client& client = clients[fd];

    // Loop on args to get the channel names, we will switch to map later
    for (std::vector<std::string>::size_type i = 1; i < args.size(); ++i) {
        std::string channelName = args[i];

        printServerMessage("INFO", "PART: " + client.getNickName() + " leaving " + channelName);
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
