/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:52:11 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/10 19:52:11 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

// I need to understand this better, cant properly inplement for now
// Kinda do now ,the logic requires freezing the registration process. until the cap command is done sending the capabilities.
void CoreServer::cmdCap(int fd, std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "CAP failed: No subcommand specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "CAP :Not enough parameters"));
        return;
    }
    std::string subcommand = args[1];
    if (subcommand == "LS") {
        std::string response = ":server CAP " + clients[fd].getNickName() + " LS :multi-prefix sasl\r\n";
        clients[fd].setResponse(response);
    } else if (subcommand == "REQ") {
        if (args.size() < 3) {
            std::cout << formatServerMessage("ERROR", "CAP REQ failed: No capability specified") << std::endl;
            clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "CAP REQ :Not enough parameters"));
            return;
        }
        std::string capability = args[2];
        if (capability == "sasl") {
            std::string response = ":server CAP " + clients[fd].getNickName() + " ACK :sasl\r\n";
            clients[fd].setResponse(response);
        } else {
            std::cout << formatServerMessage("ERROR", "CAP REQ failed: Invalid capability") << std::endl;
            clients[fd].setResponse(formatResponse(ERR_INVALIDCAP, capability + " :Invalid capability"));
        }
    } else if (subcommand == "END") {
        std::string response = ":server CAP " + clients[fd].getNickName() + " ACK :multi-prefix sasl\r\n";
        clients[fd].setResponse(response);
    } else {
        std::cout << formatServerMessage("ERROR", "CAP failed: Invalid subcommand") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_INVALIDCAPCMD, subcommand + " :Invalid CAP subcommand"));
    }
}
