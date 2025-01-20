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


void static displayTable(const std::vector<Channel>& channels) {
    std::cout << formatServerMessage("INFO", "+ Channel Table") << std::endl;
    std::cout << formatServerMessage("INFO", "+ --------------------") << std::endl;
    std::cout << formatServerMessage("INFO", "+ Name\t\tClient Count") << std::endl;
    std::cout << formatServerMessage("INFO", "+-----\t\t------------") << std::endl;
    std::ostringstream table;
    for (std::vector<Channel>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        table << "+" << it->getName() << "\t\t" << it->getMembers().size();
        std::cout << formatServerMessage("INFO", table.str()) << std::endl;
        table.str("");
    }
}

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
  return !channelName.empty() &&
         (channelName[0] == '#' || channelName[0] == '&');
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

  std::cout << formatServerMessage("DEBUG", "Channel: " + params.channels +
                                                " Key: " + params.keys)
            << std::endl;
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
  std::string joinMsg = ":" + source + " JOIN " + channelName + "\r\n";
  std::cout << formatServerMessage("SERVER", joinMsg) << std::endl;
}

void CoreServer::joinChannel(Client &client, const std::string &channelName) {
  if (!isChannel(channelName)) {
    std::cout << formatServerMessage("FATAL", "=================================" + channelName) << std::endl;
    channels.push_back(Channel(channelName));
    channels.back().addMember(client); // Access the copied object
    displayTable(channels);
    Channel &channel = channels.back();
    constructJoinMessage(client.getSource(), channelName);
    return;
  }
  Channel &channel = getChannel(channelName, channels);

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
  constructJoinMessage(client.getSource(), channelName);
}

void CoreServer::joinChannel(Client &client, const std::string &channelName,
                             const std::string &key) {
  if (!isChannel(channelName)) {

    Channel newChannel(channelName, "", key);
    newChannel.addMember(client);
    channels.push_back(newChannel);
    Channel &channel = newChannel;
    constructJoinMessage(client.getSource(), channelName);
    return;
  }
  Channel &channel = getChannel(channelName, channels);

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
  if (channel.hasPassword() && channel.checkPassword(key)) {
    std::cout << formatServerMessage("WARNING",
                                     "JOIN failed: Invalid channel key")
              << std::endl;
    client.setResponse(formatResponse(
        ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)"));
    return;
  }
  constructJoinMessage(client.getSource(), channelName);
}

static std::string channelType(const std::string &channelName) {
    if (channelName[0] == '#') {
        return "Public";
    } else if (channelName[0] == '&') {
        return "Private";
    }
    return "Unknown";
}

// Main JOIN command handler
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

  JOIN_ARGS params = parseJoinParams(args);

  std::vector<std::string> channelNames = splitString(params.channels, ' ');
  std::vector<std::string> keys = splitString(params.keys, ' ');

  for (size_t i = 0; i < channelNames.size(); ++i) {
    const std::string &channelName = channelNames[i];
    const std::string &key = (i < keys.size()) ? keys[i] : "";
    if (channelType(channelName) == "Unknown") {
      handleInvalidChannelName(client, channelName);
      return;
    }
    if (channelType(channelName) == "Public") {
      joinChannel(client, channelName);
    } else if (channelType(channelName) == "Private") {
      joinChannel(client, channelName, key);
    }
  }
}
// =====================================================================================================================

// void CoreServer::cmdList(int fd, std::vector<std::string> &args) {
//     if (args.size() > 1) {
//         std::cout << formatServerMessage("ERROR", "LIST failed: Too many
//         arguments") << std::endl;
//         clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "LIST :Too
//         many arguments")); return;
//     }
//     std::string listMsg = ":server 321 " + clients[fd].getNickName() + "
//     Channel :Users Name\r\n"; for (std::vector<Channel>::iterator it =
//     channels.begin(); it != channels.end(); ++it) {
//         listMsg += ":server 322 " + clients[fd].getNickName() + " " +
//         it->getName() + " " + std::to_string(it->getMembers().size()) + " :No
//         topic\r\n";
//     }
//     listMsg += ":server 323 " + clients[fd].getNickName() + " :End of
//     /LIST\r\n"; clients[fd].setResponse(listMsg); std::cout <<
//     formatServerMessage("SUCCESS", "Sending channel list to " +
//     clients[fd].getNickName()) << std::endl;
// }
// =====================================================================================================================
//

// static void cmdTopic(int fd, std::vector<std::string> &args) {
//   if (args.size() < 2) {
//     std::cout << formatServerMessage("ERROR",
//                                      "TOPIC failed: No channel specified")
//               << std::endl;
//     clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "TOPIC :Not
//     enough parameters")); return;
//   }
// }
