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

static const std::string& constructPassMessage(const std::string& nick, const std::string& hostname, const std::string& code, const std::string& message) {
    static std::string msg = ":" + hostname + " " + code + " " + nick + " :" + message + "\r\n";
    return msg;
}

void CoreServer::cmdPass(int fd, std::vector<std::string> &args) {
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
    std::cout << formatServerMessage("DEBUG", "Password accepted") << std::endl;
    // 001 pass accepted
    client.setResponse(constructPassMessage(client.getNickName(), this->ServData.Host, "001", "Password Accepted"));
}
