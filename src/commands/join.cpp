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
#include <string>

static void InvalidChannelName(Client &client, const std::string &channelName) {
  std::cout << formatServerMessage(
                   "ERROR", "JOIN failed: Invalid channel name " + channelName)
            << std::endl;
  client.setResponse(
      formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

static void InvalidChannelsName(Client &client,
                                const std::vector<std::string> &channels) 
{
    std::string channelsStr = "";
    for (std::vector<std::string>::const_iterator it = channels.begin();
         it != channels.end(); ++it) {
        channelsStr += *it + " ";
    }
    std::cout << formatServerMessage(
                     "ERROR", "JOIN failed: Invalid channel name " + channelsStr)
                << std::endl;
    client.setResponse(
        formatResponse(ERR_NOSUCHCHAN, channelsStr + " :Invalid channel name"));
}



static void handleMultiArgJoin(Client &client,
                               const std::vector<std::string> &args) {}

// static void joinChannel(Client& client, const std::string& channelName) {
//     const Channel& channel = getChannel(channelName, channels);
//     if (channel.isMember(client.getNickName())) {
//         std::cout << formatServerMessage("ERROR", client.getNickName() + " is
//         already a member of " + channelName) << std::endl;
//         client.setResponse(formatResponse(ERR_USERONCHANNEL,
//         client.getNickName() + " " + channelName + " :is already a member of
//         " + channelName));
//     }
//     else {
//         channel.addMember(client);
//         std::string joinMsg = ":" + client.getNickName() + "!" +
//         client.getFullName() + "@localhost JOIN " + channelName + "\r\n";
//         client.setResponse(joinMsg);
//         std::cout << formatServerMessage("SUCCESS", client.getNickName() + "
//         joined " + channelName) << std::endl;
//     }
// }

void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
    if (args.size() > 2) {
        bool multiArgs = false;
    }
    else if (args.size() < 2) {
    std::cout << formatServerMessage("ERROR",
                                     "JOIN failed: No channel specified")
              << std::endl;
    clients[fd].setResponse(
        formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
    return;
  }

  // core join logic
  std::string channelName = args[1];
  if (!isChannel(channelName)) {
    InvalidChannelName(clients[fd], channelName);
  } else {
    Client &client = clients[fd];
    std::cout << formatServerMessage("DEBUG", client.getNickName() +
                                                  " attempting to join " +
                                                  channelName)
              << std::endl;
    const Channel &channel = getChannel(channelName, channels);
    if (channel.isMember(client.getNickName())) {
      std::cout << formatServerMessage("ERROR", client.getNickName() +
                                                    " is already a member of " +
                                                    channelName)
                << std::endl;
      client.setResponse(formatResponse(
          ERR_USERONCHANNEL, client.getNickName() + " " + channelName +
                                 " :is already a member of " + channelName));
    } else {
      // joinChannel(client, channelName);
      // Find or create channel
      bool channelExists = false;
      for (std::vector<Channel>::iterator it = channels.begin();
           it != channels.end(); ++it) {
        if (it->getName() == channelName) {
          channelExists = true;
          it->addMember(client);
          break;
        }
      }

      if (!channelExists) {
        std::cout << formatServerMessage("DEBUG",
                                         "Creating new channel: " + channelName)
                  << std::endl;
        Channel newChannel(channelName);
        newChannel.addMember(client);
        channels.push_back(newChannel);
      }

      // Send join message
      std::string joinMsg = ":" + client.getNickName() + "!" +
                            client.getFullName() + "@localhost JOIN " +
                            channelName + "\r\n";
      client.setResponse(joinMsg);

      std::cout << formatServerMessage("SUCCESS", client.getNickName() +
                                                      " joined " + channelName)
                << std::endl;
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
