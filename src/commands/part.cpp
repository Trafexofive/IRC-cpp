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

static std::vector<std::string> splitString(const std::string &str, char delim) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(str);
    std::string token;
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

static void handleInvalidChannel(Client &client, const std::string &channelName) {
    printServerMessage("ERROR", "PART failed: Invalid channel");
    client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
}


void CoreServer::cmdPart(int fd, std::vector<std::string>& args) {
    if (isClientDisconnected(fd)) {
        return;
    }
    if (args.size() < 3) {
        printServerMessage("ERROR", "PART failed: Not enough parameters");
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PART :Not enough parameters"));
        return;
    }
    Client& client = clients[fd];

    std::string reason = args.size() > 3 ? args[2] : "";
    if (!reason.empty() && reason[0] == ':') {
        reason = reason.substr(1);
    }
    std::vector<std::string> channels = splitString(args[1], ',');
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        const std::string& channelName = *it;

        if (!isChannel(channelName)) {
            handleInvalidChannel(client, channelName);
            continue;
        }
        Channel* channel = getChannel(channelName);

        if (channel) {
            channel->removeMember(&client);
            this->broadcast(*channel, formatBroadcastMessage(client.getTarget(), "PART", channel->getName()));
        } else {
            printServerMessage("ERROR", "PART failed: Channel Not Found");
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
        }
    }
}
