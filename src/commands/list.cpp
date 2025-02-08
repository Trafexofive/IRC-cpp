/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 16:26:20 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/02/03 16:26:20 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void CoreServer::cmdList(int fd, std::vector<std::string> &args) {
  if (isClientDisconnected(fd)) {
    std::cout << formatServerMessage("ERROR",
                                     "LIST failed: Client is disconnected")
              << std::endl;
    return;
  }
  if (!isClientRegistered(fd)) {
    std::cout << formatServerMessage("ERROR",
                                     "LIST failed: Client not registered")
              << std::endl;
    clients[fd].setResponse(
        formatResponse(ERR_NOTREGISTERED, ":You have not registered"));
    return;
  }
  if (args.size() > 1) {
    std::cout << formatServerMessage("ERROR", "LIST failed: Too many arguments")
              << std::endl;
    clients[fd].setResponse(
        formatResponse(ERR_NEEDMOREPARAMS, "LIST :Too many arguments"));
    return;
  }
  Client &client = clients[fd];
  std::ostringstream response;

  response << ":WeUseArch" << " 321 " << client.getNickName()
           << " Channel :Users Name" << std::endl;

  for (std::map<std::string, Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    response << ":WeUseArch" << " 322 " << client.getNickName() << " "
             << it->second.getName() << " " << it->second.getMemberCount()
             << " :" << it->second.getTopic() << std::endl;
  }

  response << ":WeUseArch" << " 323 " << client.getNickName()
           << " :End of /LIST" << std::endl;

  client.setResponse(response.str());
}
