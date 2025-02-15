/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 16:59:39 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/09 16:59:39 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <string>

void CoreServer::cmdPass(int fd, std::vector<std::string> &args) {
    if (isClientDisconnected(fd)) {
        return;
    }
    if (clients[fd].isRegistered()) {
        printServerMessage("ERROR", "PASS command failed: Client already registered");
        clients[fd].setResponse(formatResponse(ERR_ALREADYREG, ":You may not reregister"));
        return;
    }
    if (args.size() < 2) {
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters"));
        return;
    }
    Client& client = clients[fd];

    if (!validatePassword(args[1])) {
        clients[fd].setResponse(formatResponse(ERR_PASSWDMISMATCH, ":Password incorrect"));
        return;
    }
    client.setPassWord(args[1]);
    client.setStatus(STATUS::AUTHENTICATED);
}
