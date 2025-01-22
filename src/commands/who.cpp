/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 15:23:07 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/21 15:23:07 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


// #define RPL_WHOREPLY (352)
// #define RPL_ENDOFWHO (315)



void CoreServer::cmdWho(int fd, std::vector<std::string> &args)
{
    Client &client = clients[fd];
    if (args.size() < 2)
    {
        client.setResponse(
            formatResponse(ERR_NEEDMOREPARAMS, "WHO :Not enough parameters"));
        return;
    }
    else if (args.size() > 2)
    {
        client.setResponse(
            formatResponse(ERR_NEEDMOREPARAMS, "WHO :Too many parameters"));
        return;
    }
    if (args[1][0] == '#')
    {
        Channel &channel = getChannel(args[1], this->channels);
        for (int i = 0; i < channel.getMembers().size(); i++)
        {
            client.setResponse(formatServerMessage(RPL_WHOREPLY, channel.getMembers()[i].getNickName()));
        }
        client.setResponse(formatServerMessage(RPL_ENDOFWHO, "End of /WHO list"));



    } else if (this->channels[])
}
