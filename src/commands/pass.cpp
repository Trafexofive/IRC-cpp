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

void CoreServer::cmdPass(int fd, std::vector<std::string> &args) {
    std::cout << formatServerMessage("DEBUG", "Processing PASS command") << std::endl;

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

    Client& client = clients[fd];
    client.setPassWord(args[1]);
    client.setAuth(true);
    std::cout << formatServerMessage("DEBUG", "Password accepted") << std::endl;
    client.setResponse(":server NOTICE Auth :Password accepted\r\n");
}
