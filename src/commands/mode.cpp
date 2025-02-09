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

#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOPRIVILEGES "481"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CHANOPRIVSNEEDED "482"
#define RPL_CHANNELMODEIS "324"
#define RPL_UMODEIS "221"

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

      if (mode[0] == '+') {
        for (size_t i = 1; i < mode.size(); i++) {
          if (mode[i] == 'o') {
            if (args.size() < 4) {
              clients[fd].setResponse(formatResponse(
                  ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
              return;
            }
            std::string nick = args[3];
            if (channel->isOperator(&clients[fd])) {
              clients[fd].setResponse(
                  formatResponse(ERR_CHANOPRIVSNEEDED,
                                 nick + " :is already a channel operator"));
              return;
            }
            channel->addOperator(&clients[fd]);
            clients[fd].setResponse(formatResponse(RPL_UMODEIS, "+o " + nick));
            return;
          } else if (mode[i] == 'i') {
            channel->setInviteMode(true);
            clients[fd].setResponse(formatResponse(RPL_UMODEIS, "+i"));
            return;
          } else if (mode[i] == 't') {
            channel->setTopicMode(true);
            clients[fd].setResponse(formatResponse(RPL_UMODEIS, "+t"));
            return;
          } else if (mode[i] == 'k') {
            if (args.size() < 4) {
              clients[fd].setResponse(formatResponse(
                  ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
              return;
            }
            std::string key = args[3];
            channel->setPassword(key);
            clients[fd].setResponse(formatResponse(RPL_UMODEIS, "+k " + key));
            return;
          }
        }
      } else if (mode[0] == '-') {
        for (size_t i = 1; i < mode.size(); i++) {
          if (mode[i] == 'o') {
            if (args.size() < 4) {
              clients[fd].setResponse(formatResponse(
                  ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
              return;
            }
            std::string nick = args[3];
            if (!channel->isOperator(&clients[fd])) {
              clients[fd].setResponse(formatResponse(
                  ERR_CHANOPRIVSNEEDED, nick + " :is not a channel operator"));
              return;
            }
            channel->removeOperator(&clients[fd]);
            clients[fd].setResponse(formatResponse(RPL_UMODEIS, "-o " + nick));
            return;
          }
        }
      }
    }
  }
}
