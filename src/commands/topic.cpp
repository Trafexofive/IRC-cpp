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

static bool containsMessage(const std::string &topic) {
  if (topic[0] == ':' && topic.size() > 1)
    return true;
  return false;
}

static std::string &appendArgs(std::vector<std::string> &args, std::string &result, int i) {
  for (;i < args.size(); i++) {
    result += args[i];
    if (i + 1 < args.size())
      result += " ";
  }
printServerMessage("DEBUG", "parsedMessage TOPIC: " + result);
  return result;
}

void CoreServer::cmdTopic(int fd, std::vector<std::string> &args) {
  if (isClientDisconnected(fd)) {
    printServerMessage("DEBUG", "TOPIC failed: Client is disconnected");
    return;
  }
  if (args.size() < 2) {
    printServerMessage("DEBUG", "TOPIC failed: Not enough parameters");
    clients[fd].setResponse(
        formatResponse(ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters"));
    return;
  }

  Client &client = clients[fd];
  std::string channelName = args[1];
  Channel *channel = getChannel(channelName);

  if (channel == NULL) {
    printServerMessage("DEBUG", "TOPIC failed: Channel does not exist");
    client.setResponse(
        formatResponse(ERR_NOSUCHCHANNEL, channelName + " :No such channel"));
    return;
  }
  if (args.size() == 2) {
    std::string topic = channel->getTopic();
    if (topic.empty()) {
      client.setResponse(
          formatResponse(RPL_NOTOPIC, channelName + " :No topic is set"));
      return;
    }
    client.setResponse(formatResponse(RPL_TOPIC, channelName + " :" + topic));
    return;
  } else if (args.size() > 2) {

    std::string parsedMessage = "";
    appendArgs(args, parsedMessage, 2);
    if (containsMessage(parsedMessage)) {

    std::string newTopic = parsedMessage.substr(1);
      channel->setTopic(newTopic);

      std::string topicMsg =
          formatResponse(RPL_TOPIC, channelName + " :" + newTopic);
      client.setResponse(topicMsg);
      printServerMessage("DEBUG", "TOPIC: " + client.getNickName() +
                                      " changed the topic of " + channelName +
                                      " to " + newTopic);
      return;
    } else {
        client.setResponse(
            formatResponse(RPL_TOPIC, channelName + " :" + "Resetting Channel topic"));
        channel->setTopic("");
    }
  }
}
