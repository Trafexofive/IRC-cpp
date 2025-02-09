/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 10:56:10 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/21 10:56:10 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void CoreServer::cmdMode(int fd, std::vector<std::string> &args) {
  if (isClientDisconnected(fd)) {
    return;
  }
  if (args.size() < 2) {
    clients[fd].setResponse(
        formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
    return;
  }
  // else if (!isOperator(fd)) {
  //     clients[fd].setResponse(
  //         formatResponse(ERR_NOPRIVILEGES, ":Permission Denied- You're not an
  //         operator"));
  //     return;
  // }

  std::string target = args[1];
  // channel mode
  if (target[0] == '#') {
    Channel *channel = getChannel(target);
    if (channel == NULL) {
      printServerMessage("ERROR", "MODE failed: Channel not found");
      clients[fd].setResponse(
          formatResponse(ERR_NOSUCHCHANNEL, target + " :No such channel"));
      return;
    } else {
      if (args.size() == 2) {
        std::string mode = channel->getMode();
        clients[fd].setResponse(
            formatResponse(RPL_CHANNELMODEIS, target + " " + mode));
        return;
      }
      std::string mode = args[2];
      // case 'k':
      //   if (args.size() < 4) {
      //     std::cout << formatServerMessage(
      //                      "ERROR", "MODE failed: Incomplete message")
      //               << std::endl;
      //     clients[fd].setResponse(formatResponse(
      //         ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
      //     return;
      //   }
      //   std::string key = args[3];
      //   channel->setKey(key);
      //   break;
      // } else {
      //   std::cout << formatServerMessage("ERROR", "MODE failed: Invalid
      //   mode")
      //             << std::endl;
      //   clients[fd].setResponse(
      //       formatResponse(ERR_UNKNOWNMODE, ":Unknown MODE flag"));
      // }
    }
  }
  // still need to align with the client class
  // else //user mode
  // {
  //     Client &client = clients[fd];
  //     if (args.size() < 3)
  //     {
  //         std::cout << formatServerMessage("ERROR", "MODE failed: Incomplete
  //         message") << std::endl;
  //         client.setResponse(formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not
  //         enough parameters")); return;
  //     }
  //     std::string mode = args[2];
  //     if (mode[0] == '+' || mode[0] == '-')
  //     {
  //         for (size_t i = 1; i < mode.size(); i++)
  //         {
  //             switch (mode[i])
  //             {
  //             case 'i':
  //                 client.setInvisible(mode[0] == '+');
  //                 break;
  //             case 'w':
  //                 client.setWallops(mode[0] == '+');
  //                 break;
  //             case 'o':
  //                 client.setOperator(mode[0] == '+');
  //                 break;
  //             case 'r':
  //                 client.setRestricted(mode[0] == '+');
  //                 break;
  //             default:
  //                 break;
  //             }
  //         }
  //     }
  //     else
  //     {
  //         std::cout << formatServerMessage("ERROR", "MODE failed: Invalid
  //         mode") << std::endl;
  //         client.setResponse(formatResponse(ERR_UMODEUNKNOWNFLAG, ":Unknown
  //         MODE flag"));
  //     }
  // }
}
