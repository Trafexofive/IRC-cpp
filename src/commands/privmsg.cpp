/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:50:09 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/10 19:50:09 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


void CoreServer::cmdPrivmsg(int fd, std::vector<std::string> &args) {
    if (args.size() < 3) {
        std::cout << formatServerMessage("ERROR", "PRIVMSG failed: Incomplete message") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters"));
        return;
    }

    std::string target = args[1];
    std::string message;
    for (std::vector<std::string>::iterator it = args.begin() + 2; it != args.end(); ++it) {
        if (it != args.begin() + 2)
            message += " ";
        message += *it;
    }

    Client& client = clients[fd];
    std::cout << formatServerMessage("MESSAGE", client.getNickName() + " -> " + target + ": " + message) << std::endl;

    std::string response = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        // Channel message
        bool channelFound = false;
        for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
            if (it->getName() == target) {
                channelFound = true;
                const std::vector<Client> &members = it->getMembers();
                for (std::vector<Client>::const_iterator member = members.begin(); member != members.end(); ++member) {
                    if (member->getNickName() != client.getNickName()) {
                        std::map<int, Client>::iterator clientIt = clients.find(member->getFd());
                        if (clientIt != clients.end())
                            clientIt->second.setResponse(response);
                    }
                }
                break;
            }
        }

        if (!channelFound)
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, target + " :No such channel"));
    } else {
        // Private message
        bool recipientFound = false;
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickName() == target) {
                recipientFound = true;
                it->second.setResponse(response);
                break;
            }
        }

        if (!recipientFound)
            client.setResponse(formatResponse(ERR_NOSUCHNICK, target + " :No such nick/channel"));
    }
}
