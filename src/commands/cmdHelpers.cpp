/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdHelpers.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 14:17:29 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/02 14:17:29 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

// Helper function to check if a name is a valid channel
bool isChannel(const std::string& name) {
    return (!name.empty() && (name[0] == '#' || name[0] == '&'));
}

// Helper function to get a channel by name
Channel& getChannel(const std::string& name, std::vector<Channel>& channels) {
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == name)
            return *it;
    }
    throw std::runtime_error("Channel not found");
}

// Helper function to remove a client from a channel
void removeClientFromChannel(Client& client, Channel& channel) {
    channel.removeMember(client.getNickName());
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

