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


void CoreServer::cmdCap(int fd, std::vector<std::string> &args) {
    if (isClientDisconnected(fd)) {
        std::cout << formatServerMessage("ERROR", "CAP failed: Client is disconnected") << std::endl;
        return;
    }
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "CAP failed: No subcommand specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "CAP :Not enough parameters"));
        return;
    } 

    std::string subcommand = args[1];

    if (subcommand != "LS" && subcommand != "END") {
        std::cout << formatServerMessage("ERROR", "CAP failed: Invalid subcommand") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_UNKNOWNCOMMAND, "CAP :Unknown subcommand"));
        return;
    } else if (subcommand == "LS") {
        clients[fd].setResponse("CAP * LS :multi-prefix");
    } else if (subcommand == "END") {
        clients[fd].setResponse("CAP * ACK :multi-prefix");
        // clients[fd].setPending();
    }
}
