/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 00:19:13 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/18 00:19:13 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

static std::string constructQuitMessage(const Client &client, const std::vector<std::string> &args) {
    std::string quitMsg = ":WeUseArch " + client.getTarget() + " QUIT :";
    if (args.size() > 1) {
        std::vector<std::string>::const_iterator it = args.begin() + 1;
        for (; it != args.end(); ++it) {
            if (it != args.begin() + 1) {
                quitMsg += " ";
            }
            quitMsg += *it;
        }
    }
    quitMsg += "\r\n";
    return quitMsg;
}

void CoreServer::cmdQuit(int fd, std::vector<std::string> &args) {
    if (isClientDisconnected(fd)) {
        printServerMessage("ERROR", "QUIT: Client already disconnected");
        return;
    }
    Client &client = clients[fd];

    std::string quitMsg = constructQuitMessage(client, args);
    client.setResponse(quitMsg);
    this->WriteEvent(fd);
    printServerMessage("INFO", "QUIT: " + client.getNickName() + " has quit");

    handleDisconnect(fd);
}



