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
    if (clients[fd].getConnected()) {
        std::cout << formatServerMessage("WARNING", "PASS command failed: Client already authenticated") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_ALREADYREG, ":You may not reregister"));
        return;
    }
    std::cout << formatServerMessage("DEBUG", "Processing PASS command") << std::endl;
    Client& client = clients[fd];

    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "PASS command failed: Not enough parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters"));
        return;
    }
    if (args[1] != ServData.Passwd) {
        std::cout << formatServerMessage("ERROR", "PASS command failed: Invalid password") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_PASSWDMISMATCH, ":Password incorrect"));
        return;
    }
    client.setPassWord(args[1]);
    client.setAuth(true);
    std::cout << formatServerMessage("INFO", "Client Authenticated") << std::endl;
    
}
