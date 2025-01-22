/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 17:29:11 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/06 17:29:11 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <iostream>


typedef struct {
  std::string channels;
  std::string keys;
} JOIN_ARGS;

static void handleInvalidChannelName(Client &client,
                                     const std::string &channelName) {
  std::cout << formatServerMessage("WARNING",
                                   "JOIN failed: Invalid channel name " +
                                       channelName)
            << std::endl;
  client.setResponse(
      formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

static bool isValidChannelName(const std::string &channelName) {
  return !channelName.empty() && (channelName[0] == '#');
}

// Helper function to split a string into a vector of strings
static std::vector<std::string> splitString(const std::string &input,
                                            char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream stream(input);
  std::string token;
  while (std::getline(stream, token, delimiter)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}

static JOIN_ARGS parseJoinParams(const std::vector<std::string> &args) {
  JOIN_ARGS params;

  if (args.size() < 2) {
    return params;
  }

  params.channels = args[1];

  if (args.size() > 2) {
    params.keys = args[2];
  }

  for (size_t i = 0; i < params.channels.length(); ++i) {
    if (params.channels[i] == ',') {
      params.channels[i] = ' ';
    }
  }

  for (size_t i = 0; i < params.keys.length(); ++i) {
    if (params.keys[i] == ',') {
      params.keys[i] = ' ';
    }
  }
  return params;
}

#define ERR_BADCHANNELKEY "475"

bool CoreServer::isChannel(const std::string &name) {

  for (std::vector<Channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (it->getName() == name)
      return true;
  }
  return false;
}

static void constructJoinMessage(const std::string &source,
                                 const std::string &channelName) {
  std::string joinMsg = ":" + source + " JOIN :" + channelName;
  std::cout << formatServerMessage("SERVER", joinMsg) << std::endl;
}

void CoreServer::joinChannel(Client &client, const std::string &channelName) {
  if (!isChannel(channelName)) {
    channels.push_back(Channel(channelName));
    channels.back().addMember(client);
    constructJoinMessage(client.getSource(), channelName);
    return;
  }
  Channel &channel = getChannel(channelName, channels);

  if (channel.getType() == "PRIVATE" &&
      !channel.isMember(client.getNickName())) {
    std::cout << formatServerMessage("WARNING",
                                     "JOIN failed: Channel requires a key")
              << std::endl;
    client.setResponse(formatResponse(
        ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)"));
    return;
  }
  if (channel.isMember(client.getNickName())) {
    std::cout << formatServerMessage("WARNING", client.getNickName() +
                                                    " is already in channel " +
                                                    channelName)
              << std::endl;
    client.setResponse(formatResponse(ERR_USERONCHANNEL,
                                      client.getNickName() + " " + channelName +
                                          " :is already on channel"));
    return;
  }
  channel.addMember(client);
  constructJoinMessage(client.getSource(), channelName);
}

void CoreServer::joinChannel(Client &client, const std::string &channelName,
                             const std::string &key) {
  if (!isChannel(channelName)) {

    channels.push_back(Channel(channelName, "", key));
    channels.back().addMember(client);
    constructJoinMessage(client.getSource(), channelName);
    return;
  }
  Channel &channel = getChannel(channelName, channels);

  if (channel.getType() == "PUBLIC" &&
      !channel.isMember(client.getNickName())) {
    joinChannel(client, channelName);
    return;
  }
  if (channel.isMember(client.getNickName())) {
    std::cout << formatServerMessage("WARNING", client.getNickName() +
                                                    " is already in channel " +
                                                    channelName)
              << std::endl;
    client.setResponse(formatResponse(ERR_USERONCHANNEL,
                                      client.getNickName() + " " + channelName +
                                          " :is already on channel"));
    return;
  }
  if (channel.getType() == "PRIVATE" && !channel.checkPassword(key)) {
    std::cout << formatServerMessage("WARNING",
                                     "JOIN failed: Invalid channel key")
              << std::endl;
    client.setResponse(formatResponse(
        ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)"));
    return;
  }
  channel.addMember(client);
  constructJoinMessage(client.getSource(), channelName);
}

void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
  Client &client = clients[fd];

  if (!client.isAuthenticated()) {
    std::cout << formatServerMessage("WARNING",
                                     "JOIN failed: Client not authenticated")
              << std::endl;
    client.setResponse(
        formatResponse(ERR_NOTREGISTERED, "JOIN :You have not registered"));
    return;
  }

  if (args.size() < 2) {
    std::cout << formatServerMessage("WARNING",
                                     "JOIN failed: No channel specified")
              << std::endl;
    client.setResponse(
        formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
    return;
  }

  if (args.size() > 3) {
    std::cout << formatServerMessage("WARNING",
                                     "JOIN failed: Too many arguments")
              << std::endl;
    client.setResponse(
        formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Too many arguments"));
    return;
  }

  // needs better error/edge-cases (parse ya lm3gaz)
  JOIN_ARGS params = parseJoinParams(args);

  std::vector<std::string> channelNames = splitString(params.channels, ' ');
  std::vector<std::string> keys = splitString(params.keys, ' ');

  if (channelNames.empty()) {
    client.setResponse(formatResponse(ERR_NEEDMOREPARAMS,
                                      "JOIN :No valid channels specified"));
    return;
  }

  for (size_t i = 0; i < channelNames.size(); ++i) {
    if (!isValidChannelName(channelNames[i])) {
      handleInvalidChannelName(client, channelNames[i]);
      continue;
    }

    std::string key;
    if (i < keys.size()) {
      key = keys[i];
    }

    if (key.empty()) {
      joinChannel(client, channelNames[i]);
    } else {
      joinChannel(client, channelNames[i], key);
    }
  }
}
