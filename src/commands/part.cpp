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

void CoreServer::cmdPart(int fd, std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "PART failed: No channel specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PART :Not enough parameters"));
        return;
    }

    // Check if client exists
    if (clients.find(fd) == clients.end()) {
        std::cout << formatServerMessage("ERROR", "PART failed: Client not found") << std::endl;
        return;
    }

    Client& client = clients[fd];

    // Loop on args to get the channel names
    for (std::vector<std::string>::size_type i = 1; i < args.size(); ++i) {
        std::string channelName = args[i];

        if (!isChannel(channelName)) {
            handleInvalidChannel(client, channelName);
            continue;
        }

        std::cout << formatServerMessage("DEBUG", client.getNickName() + " attempting to leave " + channelName) << std::endl;

        try {
            Channel& channel = getChannel(channelName, channels);
            removeClientFromChannel(client, channel);
            handlePartSuccess(client, channelName);
        } catch (const std::runtime_error& e) {
            std::cout << formatServerMessage("ERROR", "PART failed: " + std::string(e.what())) << std::endl;
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
        }
    }
}
