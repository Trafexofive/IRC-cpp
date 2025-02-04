/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 01:37:39 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/02/02 01:37:39 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


void CoreServer::cmdTopic(int fd, std::vector<std::string> &args) {
  if (isClientDisconnected(fd)) {
    std::cout << formatServerMessage("DEBUG", "TOPIC failed: Client is disconnected") << std::endl;
    return;
  }
  if (!isClientRegistered(fd)) {
    std::cout << formatServerMessage("DEBUG", "TOPIC failed: Client not registered") << std::endl;
    clients[fd].setResponse(formatResponse(ERR_NOTREGISTERED, ":You have not registered"));
    return;
  }
  if (args.size() < 2) {
    std::cout << formatServerMessage("DEBUG", "TOPIC failed: No channel specified") << std::endl;
    clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters"));
    return;
  }
  Client &client = clients[fd];
  std::string channelName = args[1];
  Channel *channel = getChannel(channelName);
  if (channel == NULL) {
    std::cout << formatServerMessage("DEBUG", "TOPIC failed: Channel not found") << std::endl;
    return;
  }
  if (args.size() == 2) {
    std::string topic = channel->getTopic();
    client.setResponse(formatResponse(RPL_TOPIC, channelName + " :" + topic));
    return;
  }
  std::string newTopic = args[2];
  channel->setTopic(newTopic);
  std::string topicMsg = ":" + client.getNickName() + "!" +
                         client.getFullName() + "@localhost TOPIC " +
                         channelName + " :" + newTopic + "\r\n";
  client.setResponse(topicMsg);
  std::cout << formatServerMessage("SUCCESS", client.getNickName() + " changed the topic of " +
                                                channelName + " to " + newTopic)
            << std::endl;
}
