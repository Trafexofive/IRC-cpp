/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 17:00:05 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/09 17:00:05 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void CoreServer::cmdNick(int fd, std::vector<std::string> &args) {
  if (isClientDisconnected(fd) || !isClientAuthenticated(fd)) {
    return;
  }
  if (args.size() < 2) {
    printServerMessage("ERROR", "NICK command failed: No nickname given");
    clients[fd].setResponse(formatResponse("431", ":No nickname given"));
    return;
  }

  const std::string &nickname = args[1];

  // Check for invalid characters
  if (nickname.find_first_of(" ,*?!@") != std::string::npos) {
    std::cout << formatServerMessage(
                     "ERROR",
                     "NICK command failed: Invalid characters in nickname")
              << std::endl;
    clients[fd].setResponse(
        formatResponse("432", "* " + nickname + " :Erroneous nickname"));
    return;
  }

  // Check for duplicate nicknames
  for (std::map<int, Client>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    if (it->first != fd && it->second.getNickName() == nickname) {
        printServerMessage("ERROR", "NICK command failed: Nickname already in use");
      clients[fd].setResponse(formatResponse(
          "433", "* " + nickname + " :Nickname is already in use"));
      return;
    }
  }

  Client &client = clients[fd];

  std::string oldNick = client.getNickName();
  client.setNickName(nickname);

  if (oldNick.empty() && client.isAuthenticated()) {

    std::string response = ":" + client.getTarget() + " 001 " + nickname +
                           " :Welcome to the Internet Relay Network " + nickname +
                           "\r\n";
    client.setResponse(response);
    printServerMessage("INFO", "Client registered @fd: " + numberToString(fd));
  } else if (isClientRegistered(fd)) {
    printServerMessage("INFO", "Client changed nickname @fd: " + numberToString(fd));
    std::string response =
        ":" + client.getTarget() + " NICK :" + nickname + "\r\n";
    client.setResponse(response);
  }
}
