/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 16:57:41 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/09 16:57:41 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

static void printClientInfo(Client &client) {
    std::cout << "Client info:" << std::endl;
    std::cout << "Nickname: " << client.getNickName() << std::endl;
    std::cout << "Username: " << client.getFullName() << std::endl;
    std::cout << "Realname: " << client.getRealName() << std::endl;
    std::cout << "Auth: " << client.getAuth() << std::endl;
}

void CoreServer::cmdUser(int fd, std::vector<std::string> &args) {
    std::cout << formatServerMessage("DEBUG", "Processing USER command") << std::endl;

    if (args.size() < 5) {
        std::cout << formatServerMessage("ERROR", "USER command failed: Not enough parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "USER :Not enough parameters"));
        return;
    }

    Client& client = clients[fd];

    if (!client.getFullName().empty()) {
        std::cout << formatServerMessage("ERROR", "USER command failed: Already registered") << std::endl;
        client.setResponse(formatResponse(ERR_ALREADYREG, ":You may not reregister"));
        return;
    }

    std::string username = args[1];
    client.setFullName(username);

    if (client.getAuth() && !client.getNickName().empty()) {
        std::string nick = client.getNickName();
        std::cout << formatServerMessage("DEBUG", "Registration complete for " + nick) << std::endl;

        // Send welcome messages using formatResponse
        client.setResponse(formatResponse(RPL_WELCOME, nick + " :Welcome to the WeUseArch IRC Network"));
        client.setResponse(formatResponse(RPL_YOURHOST, nick + " :Your host is morpheus-server.ddns.net"));
        printClientInfo(client);
    }
}
