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
#include <iostream>
#include <string>

static void cmdMotd(Client& client, std::vector<std::string> &args) {
    std::string nick = client.getNickName();

    // Example MOTD response
    client.setResponse(formatResponseHost(RPL_MOTDSTART, nick + " :- morpheus-server.ddns.net Message of the Day - "));
    client.setResponse(formatResponseHost(RPL_MOTD, nick + " :- Welcome to the WeUseArch IRC Network!"));
    client.setResponse(formatResponseHost(RPL_MOTD, nick + " :- Enjoy your stay!"));
    client.setResponse(formatResponseHost(RPL_ENDOFMOTD, nick + " :End of /MOTD command."));
}

static void welcomeClient(Client& client) {
    std::string nick = client.getNickName();
    // Send welcome messages using formatResponse
    client.setResponse(formatResponseHost(RPL_WELCOME, nick + " :Welcome to the WeUseArch IRC Network"));
    client.setResponse(formatResponseHost(RPL_YOURHOST, nick + " :Your host is morpheus-server.ddns.net"));
    client.setResponse(formatResponseHost(RPL_CREATED, nick + " :This server was created 2024-01-11"));
    client.setResponse(formatResponseHost(RPL_MYINFO, nick + " morpheus-server.ddns.net 1.0 usermodes channelmodes"));
    // client.setResponse(formatResponse(RPL_ISUPPORT, nick + " CHANTYPES=# PREFIX=(o)@ CHANMODES=k,l,imnpst :are supported by this server"));
    // Send MOTD
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
    std::string realname = args[4];
    // int n = 2;
    // while (!args[n].empty()) {
    //     n++;
    // }
    client.setFullName(username);
    realname.erase(std::remove(realname.begin(), realname.end(), ':'), realname.end());
    client.setRealName(realname);

    if (client.getAuth() && !client.getNickName().empty()) {
        std::string nick = client.getNickName();

        std::cout << formatServerMessage("INFO", "Registration complete for " + client.getSource()) << std::endl;
        client.setConnected(true);
        client.constructSource();
        client.printClientInfo();

        welcomeClient(client);
        cmdMotd(client, args);

        // Handle client modes
        // if (!client.getModes().empty()) {
        //     client.setResponse(formatResponse(RPL_UMODEIS, nick + " " + client.getModes()));
        // }

    }
}


