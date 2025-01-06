/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 17:29:11 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/06 17:29:11 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "JOIN failed: No channel specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
        return;
    }

    std::string channelName = args[1];
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    Client& client = clients[fd];
    std::cout << formatServerMessage("DEBUG", client.getNickName() + " attempting to join " + channelName) << std::endl;

    // Find or create channel
    bool channelExists = false;
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == channelName) {
            channelExists = true;
            it->addMember(client);
            break;
        }
    }

    if (!channelExists) {
        std::cout << formatServerMessage("DEBUG", "Creating new channel: " + channelName) << std::endl;
        Channel newChannel(channelName);
        newChannel.addMember(client);
        channels.push_back(newChannel);
    }

    // Send join message
    std::string joinMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost JOIN " + channelName + "\r\n";
    client.setResponse(joinMsg);

    std::cout << formatServerMessage("SUCCESS", client.getNickName() + " joined " + channelName) << std::endl;
}
