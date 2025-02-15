/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/18 00:19:13 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/18 00:19:13 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

// #define QUIT_USAGE "QUIT: Usage: /quit [reason]"
#define formatQuit(target, message)                               \
  ":" + target + " QUIT: " + message + "\r\n"
#define formatQuitNoReason(target) ":" + target + " QUIT:Quit: \r\n"

static std::string constructQuitMessage(const Client &client,
                                        const std::vector<std::string> &args) {
  std::string quitMsg;
  if (args.size() == 1) {
    quitMsg = formatQuitNoReason(client.getTarget());
    return quitMsg;
  }
  if (args.size() > 1) {

    std::vector<std::string>::const_iterator it = args.begin() + 1;
    for (; it != args.end(); ++it) {
      if (it != args.begin() + 1) {
        quitMsg += " ";
      }
      quitMsg += *it;
    }
    quitMsg = formatQuit(client.getTarget(), quitMsg);
  } 
  return quitMsg;
}

void CoreServer::cmdQuit(int fd, std::vector<std::string> &args) {
  if (isClientDisconnected(fd)) {
    printServerMessage("ERROR", "QUIT: Client already disconnected");
    return;
  }
  Client &client = clients[fd];

  std::string quitMsg = constructQuitMessage(client, args);
  printServerMessage("INFO", "quit msg: " + quitMsg);
  // this->broadcastSubbedChannels(quitMsg, &client);
  client.setResponse(quitMsg);
  this->broadcastChannelsException(quitMsg, &client);

  // printServerMessage("WARNING", "QUIT: " + client.getNickName() + " has quit");

  handleDisconnect(fd);
}
