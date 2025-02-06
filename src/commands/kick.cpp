/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 19:57:12 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/02/06 19:57:12 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


void CoreServer::cmdKick(int fd, std::vector<std::string> &args)
{
    if (isClientDisconnected(fd))
        return;
    Client &client = clients[fd];
    if (args.size() < 3)
    {
        client.setResponse(formatResponse(ERR_NEEDMOREPARAMS, "KICK :Not enough parameters"));
        return;
    }
    else if (args.size() > 4)
    {
        client.setResponse(formatResponse(ERR_NEEDMOREPARAMS, "KICK :Too many parameters"));
        return;
    }
    else if (args.size() > 2) // could have reason if not display default.
    {
        Channel *channel = getChannel(args[1]);
        if (channel == NULL)
        {
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, args[1] + " :No such channel"));
            return;
        }
        if (args.size() == 3)
        {
            const std::string &target = args[2];
            if (!channel->isMember(client))
            {
            //handle client not being on channel
            // client.setResponse(formatResponse(ERR_NOTONCHANNEL, args[1] + " :You're not on that channel"));
            return;
            } else if (channel->isMember(client))
            {
            //kick target from channel
            // client.setResponse(formatResponse(ERR_CHANOPRIVSNEEDED, args[1] + " :You're not a channel operator"));
            return;
            }
        }
        else if (args.size() == 4)
        {
            const std::string &target = args[2];
            const std::string &reason = args[3];
            if (!channel->isMember(client))
            {
            //handle client not being on channel
            // client.setResponse(formatResponse(ERR_NOTONCHANNEL, args[1] + " :You're not on that channel"));
            return;
            } else if (channel->isMember(client))
            {
            //kick target from channel
            // client.setResponse(formatResponse(ERR_CHANOPRIVSNEEDED, args[1] + " :You're not a channel operator"));
            return;
            }
        }

    }
}
