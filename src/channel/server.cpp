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

