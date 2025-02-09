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
  printServerMessage("DEBUG", "JOIN failed: Invalid channel name");
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

  // Validate channel name format
  if (args[1].empty() || args[1][0] != '#') {
    return params;
  }

  params.channels = args[1];

  if (args.size() > 2) {
    params.keys = args[2];
  }

  // Convert commas to spaces
  std::replace(params.channels.begin(), params.channels.end(), ',', ' ');
  std::replace(params.keys.begin(), params.keys.end(), ',', ' ');

  return params;
}

void static handleJoinSuccess(Client &client, Channel &channel) {
  // channel.broadcast(formatBroadcastMessage(client.getTarget(), "JOIN",
  // channel.getName()));
  //   std::string topic = channel.getTopic();
  //   if (!topic.empty()) {
  //     client.setResponse(RPLTOPIC(client.getTarget(), channel.getName(),
  //     topic));
  //   }
  //   client.setResponse(RPLTOPICWHOTIME(client.getTarget(), channel.getName(),
  //                                       "WeUseArch", "0"));
  // //handle names and time
  //   client.setResponse(RPLNAMREPLY(client.getTarget(), channel.getName(),
  //                                   channel.getMemberList()));
  //   client.setResponse(RPLENDOFNAMES(client.getTarget(), channel.getName()));
}


void CoreServer::joinChannel(Client &client, const std::string &channelName) {
    Channel *channelPtr = getChannel(channelName);
    if (!channelPtr) {
        createChannel(channelName, &client);
        Channel &channel = this->channels.rbegin()->second;
        this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
        return;
    }
    Channel &channel = *channelPtr;
    if (channel.isPrivate() && !channel.isMember(client)) {
        printServerMessage("WARNING", "JOIN failed: Channel is private");
        client.setResponse(ERR_BADCHANNELKEY_MSG(client.getTarget(), channelName));
        return;
    }
    if (channel.isMember(client)) {
        printServerMessage("WARNING", client.getNickName() + " is already in channel " + channelName);
        client.setResponse(ERR_USERONCHANNEL_MSG(client.getTarget(), channelName));
        return;
    }
    channel.addMember(&client);
    this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
}

void CoreServer::joinChannel(Client &client, const std::string &channelName, const std::string &key) {

    Channel *channelPtr = getChannel(channelName);
    if (!channelPtr) {
        createChannel(channelName, "", key, &client);
        Channel &channel = this->channels.rbegin()->second;
        this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
        return;
    }
    Channel &channel = *channelPtr;
    if (channel.isMember(client)) {
        printServerMessage("WARNING", client.getNickName() + " is already in channel " + channelName);
        client.setResponse(ERR_USERONCHANNEL_MSG(client.getTarget(), channelName));
        return;
    }
    if (channel.getChannelType() == CHANNEL::PUBLIC) {
        channel.addMember(&client);
        this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
        return;
    }
    if (channel.isPrivate() && !channel.validatePassword(key)) {
        std::cout << formatServerMessage("WARNING", "JOIN failed: Invalid channel key") << std::endl;
        client.setResponse(formatResponse(ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)"));
        return;
    }
    channel.addMember(&client);
    this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
}


void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {

  if (isClientDisconnected(fd))
    return;
  Client &client = clients[fd];
  if (args.size() < 2) {
    client.setResponse(ERR_NEEDMOREPARAMS_MSG(client.getTarget(), "JOIN"));

    return;
  }

  if (args.size() > 3) {
    // client.setResponse(ERR_TOOMANYARGS_MSG(client.getTarget(), "JOIN"));
    return;
  }

  JOIN_ARGS params = parseJoinParams(args);

  // this is garbage my bad al bro, I optimized it a bit
  std::vector<std::string> channelNames = splitString(params.channels, ' ');
  std::vector<std::string> keys = splitString(params.keys, ' ');

  for (size_t i = 0; i < channelNames.size(); ++i) {
    if (!isValidChannelName(channelNames[i])) {
      handleInvalidChannelName(client, channelNames[i]);
      continue;
    }

    std::string key;
    if (i < keys.size()) {
      key = keys[i];
    }
    printServerMessage("DEBUG", "index: " + numberToString(i) + " channel: " + channelNames[i] + " key: " + key);

    if (key.empty()) {
      joinChannel(client, channelNames[i]);
    } else {
      joinChannel(client, channelNames[i], key);
    }
  }
}
