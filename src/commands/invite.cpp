/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 19:25:59 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/02/06 19:25:59 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <vector>


void CoreServer::cmdInvite(int fd, std::vector<std::string> &args)
{
    if (isClientDisconnected(fd))
        return;
    Client &client = clients[fd];

    if (args.size() < 3)
    {
        client.setResponse(formatResponse(ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters"));
        return;
    }
    else if (args.size() > 3)
    {
        client.setResponse(ERRNEEDMOREPARAMS(client.getNickName(), "INVITE"));
        return;
    }
    else 
    {
        Channel *channel = getChannel(args[1]);
        if (channel == NULL)
        {
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, args[1] + " :No such channel"));
            return;
        } else if (!channel->isMember(client))
        {
            client.setResponse(formatResponse(ERR_NOTONCHANNEL, args[1] + " :You're not on that channel"));
            return;
        }

    }

}

