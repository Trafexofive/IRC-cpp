/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 17:00:05 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/09 17:00:05 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


void CoreServer::cmdNick(int fd, std::vector<std::string> &args) {
    if (!isClientAuthenticated(fd)) {
        std::cout << formatServerMessage("ERROR", "NICK command failed: Client not found") << std::endl;
        return;
    }
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "NICK command failed: No nickname provided") << std::endl;
        clients[fd].setResponse(formatResponse("431", ":No nickname given"));
        return;
    }

    const std::string &nickname = args[1];

    // Check for invalid characters
    if (nickname.find_first_of(" ,*?!@") != std::string::npos) {
        std::cout << formatServerMessage("ERROR", "NICK command failed: Invalid characters in nickname") << std::endl;
        clients[fd].setResponse(formatResponse("432", "* " + nickname + " :Erroneous nickname"));
        return;
    }

    // Check for duplicate nicknames
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->first != fd && it->second.getNickName() == nickname) {
            std::cout << formatServerMessage("ERROR", "NICK command failed: Nickname already in use") << std::endl;
            clients[fd].setResponse(formatResponse("433", "* " + nickname + " :Nickname is already in use"));
            return;
        }
    }

    // setting the nickname
    Client& client = clients[fd];
    std::string oldNick = client.getNickName();
    client.setNickName(nickname);

    if (oldNick.empty() && client.isAuthenticated()) {
        std::cout << formatServerMessage("DEBUG", "Nickname set to: " + nickname) << std::endl;
        if (client.isAuthenticated()) {
            // Send welcome messages using formatResponse
            client.setResponse(formatResponse(RPL_WELCOME, nickname + " :Welcome to the WeUseArch IRC CHAT."));
            client.setResponse(formatResponse(RPL_YOURHOST, nickname + " :Your host is morpheus-server.ddns.net"));
            client.setResponse(formatResponse(RPL_CREATED, nickname + " :This server was created " __DATE__));
            client.setResponse(formatResponse(RPL_MYINFO, nickname + " :IRC server v1.0"));
        }
    } else {
        std::cout << formatServerMessage("DEBUG", "Nickname changed from " + oldNick + " to " + nickname) << std::endl;
        std::string response = ":" + oldNick + "!" + client.getFullName() + "@localhost NICK :" + nickname + "\r\n";
        client.setResponse(response);
    }
}
