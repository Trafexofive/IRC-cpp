/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 16:54:12 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/09 16:54:12 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void    CoreServer::cmdPing(int fd, std::vector<std::string> &args)
{
    
// Numeric Replies:
//
// -   [`ERR_NEEDMOREPARAMS`](#errneedmoreparams-461) `(461)`
// -   [`ERR_NOORIGIN`](#errnoorigin-409) `(409)`
//
// Deprecated Numeric Reply:
// -   [`ERR_NOSUCHSERVER`](#errnosuchserver-402) `(402)`
    
    if (args.size() < 2)
    {
        std::cout << formatServerMessage("ERROR", "PONG failed: No parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NOORIGIN, "PONG :No origin specified"));
        return;
    }
    Client& client = clients[fd];

    std::string response = "PONG " + args[1] + "\r\n";
    client.setResponse(response);
}
