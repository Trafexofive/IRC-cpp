/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 08:42:52 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 08:42:52 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/core_server.hpp"
#include <ctime>
#include <sstream>

void CoreServer::cmdPass(int fd, std::vector<std::string> &args) {
  std::cout << formatServerMessage("DEBUG", "Processing PASS command")
            << std::endl;

  if (args.size() < 2) {
    std::cout << formatServerMessage(
                     "ERROR", "PASS command failed: Not enough parameters")
              << std::endl;
    clients[fd].set_response(
        formatResponse(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters"));
    return;
  }

  if (args[1] != ServData.Passwd) {
    std::cout << formatServerMessage("ERROR",
                                     "PASS command failed: Invalid password")
              << std::endl;
    clients[fd].set_response(
        formatResponse(ERR_PASSWDMISMATCH, ":Password incorrect"));
    return;
  }

  clients[fd].set_pass(args[1]);
  clients[fd].set_bool(true);
  std::cout << formatServerMessage("DEBUG", "Password accepted") << std::endl;
  clients[fd].set_response(":server NOTICE Auth :Password accepted\r\n");
}

void CoreServer::cmdNick(int fd, std::vector<std::string> &args) {
  std::cout << formatServerMessage("DEBUG", "Processing NICK command")
            << std::endl;

  if (args.size() < 2) {
    std::cout << formatServerMessage(
                     "ERROR", "NICK command failed: No nickname provided")
              << std::endl;
    clients[fd].set_response(formatResponse("431", ":No nickname given"));
    return;
  }

  const std::string &nickname = args[1];

  // Check for invalid characters
  if (nickname.find_first_of(" ,*?!@") != std::string::npos) {
    std::cout << formatServerMessage(
                     "ERROR",
                     "NICK command failed: Invalid characters in nickname")
              << std::endl;
    clients[fd].set_response(
        formatResponse("432", "* " + nickname + " :Erroneous nickname"));
    return;
  }

  // Check for duplicate nicknames
  for (std::map<int, _client>::iterator it = clients.begin();
       it != clients.end(); ++it) {
    if (it->first != fd && it->second.get_nick() == nickname) {
      std::cout << formatServerMessage(
                       "ERROR", "NICK command failed: Nickname already in use")
                << std::endl;
      clients[fd].set_response(formatResponse(
          "433", "* " + nickname + " :Nickname is already in use"));
      return;
    }
  }

  std::string oldNick = clients[fd].get_nick();
  clients[fd].set_nick(nickname);

  if (oldNick.empty()) {
    std::cout << formatServerMessage("DEBUG", "Nickname set to: " + nickname)
              << std::endl;
    if (clients[fd].get_bool() && !clients[fd].get_user().empty()) {
      // Send welcome messages using formatResponse
      clients[fd].set_response(formatResponse(
          RPL_WELCOME, nickname + " :Welcome to the IRC Network"));
      clients[fd].set_response(formatResponse(
          RPL_YOURHOST, nickname + " :Your host is irc.example.com"));
      clients[fd].set_response(formatResponse(
          RPL_CREATED, nickname + " :This server was created " __DATE__));
      clients[fd].set_response(
          formatResponse(RPL_MYINFO, nickname + " :IRC server v1.0"));
    }
  } else {
    std::cout << formatServerMessage("DEBUG", "Nickname changed from " +
                                                  oldNick + " to " + nickname)
              << std::endl;
    std::string response = ":" + oldNick + "!" + clients[fd].get_user() +
                           "@localhost NICK :" + nickname + "\r\n";
    clients[fd].set_response(response);
  }
}

void CoreServer::cmdUser(int fd, std::vector<std::string> &args) {
  std::cout << formatServerMessage("DEBUG", "Processing USER command")
            << std::endl;

  if (args.size() < 5) {
    std::cout << formatServerMessage(
                     "ERROR", "USER command failed: Not enough parameters")
              << std::endl;
    clients[fd].set_response(
        formatResponse(ERR_NEEDMOREPARAMS, "USER :Not enough parameters"));
    return;
  }

  if (!clients[fd].get_user().empty()) {
    std::cout << formatServerMessage("ERROR",
                                     "USER command failed: Already registered")
              << std::endl;
    clients[fd].set_response(
        formatResponse(ERR_ALREADYREG, ":You may not reregister"));
    return;
  }

  std::string username = args[1];
  clients[fd].set_user(username);

  if (clients[fd].get_bool() && !clients[fd].get_nick().empty()) {
    std::string nick = clients[fd].get_nick();
    std::cout << formatServerMessage("DEBUG",
                                     "Registration complete for " + nick)
              << std::endl;

    // Send welcome messages using formatResponse
    clients[fd].set_response(
        formatResponse(RPL_WELCOME, nick + " :Welcome to the IRC Network"));
    clients[fd].set_response(
        formatResponse(RPL_YOURHOST, nick + " :Your host is irc.example.com"));
    clients[fd].set_response(formatResponse(
        RPL_CREATED, nick + " :This server was created " __DATE__));
    clients[fd].set_response(
        formatResponse(RPL_MYINFO, nick + " :IRC server v1.0"));
  }
}

void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
  if (args.size() < 2) {
    std::cout << formatServerMessage("ERROR",
                                     "JOIN failed: No channel specified")
              << std::endl;
    clients[fd].set_response(
        formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
    return;
  }

  std::string channelName = args[1];
  if (channelName[0] != '#')
    channelName = "#" + channelName;

  std::cout << formatServerMessage("DEBUG", clients[fd].get_nick() +
                                                " attempting to join " +
                                                channelName)
            << std::endl;

  // Find or create channel
  bool channelExists = false;
  for (std::vector<channel>::iterator it = channels.begin();
       it != channels.end(); ++it) {
    if (it->getName() == channelName) {
      channelExists = true;
      it->addMember(clients[fd]);
      break;
    }
  }

  if (!channelExists) {
    std::cout << formatServerMessage("DEBUG",
                                     "Creating new channel: " + channelName)
              << std::endl;
    channel newChannel(channelName);
    newChannel.addMember(clients[fd]);
    channels.push_back(newChannel);
  }

  // Send join message
  std::string joinMsg = ":" + clients[fd].get_nick() + "!" +
                        clients[fd].get_user() + "@localhost JOIN " +
                        channelName + "\r\n";
  clients[fd].set_response(joinMsg);

  std::cout << formatServerMessage("SUCCESS", clients[fd].get_nick() +
                                                  " joined " + channelName)
            << std::endl;
}

void CoreServer::cmdPrivmsg(int fd, std::vector<std::string> &args) {
  if (args.size() < 3) {
    std::cout << formatServerMessage("ERROR",
                                     "PRIVMSG failed: Incomplete message")
              << std::endl;
    clients[fd].set_response(
        formatResponse(ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters"));
    return;
  }

  std::string target = args[1];
  std::string message;
  for (std::vector<std::string>::iterator it = args.begin() + 2;
       it != args.end(); ++it) {
    if (it != args.begin() + 2)
      message += " ";
    message += *it;
  }

  std::cout << formatServerMessage("MESSAGE", clients[fd].get_nick() + " -> " +
                                                  target + ": " + message)
            << std::endl;

  std::string response = ":" + clients[fd].get_nick() + "!" +
                         clients[fd].get_user() + "@localhost PRIVMSG " +
                         target + " :" + message + "\r\n";

  if (target[0] == '#') {
    // Channel message
    bool channelFound = false;
    for (std::vector<channel>::iterator it = channels.begin();
         it != channels.end(); ++it) {
      if (it->getName() == target) {
        channelFound = true;
        const std::vector<_client> &members = it->getMembers();
        for (std::vector<_client>::const_iterator member = members.begin();
             member != members.end(); ++member) {
          if (member->get_nick() != clients[fd].get_nick()) {
            std::map<int, _client>::iterator clientIt =
                clients.find(member->get_fd());
            if (clientIt != clients.end())
              clientIt->second.set_response(response);
          }
        }
        break;
      }
    }

    if (!channelFound)
      clients[fd].set_response(
          formatResponse(ERR_NOSUCHCHAN, target + " :No such channel"));
  } else {
    // Private message
    bool recipientFound = false;
    for (std::map<int, _client>::iterator it = clients.begin();
         it != clients.end(); ++it) {
      if (it->second.get_nick() == target) {
        recipientFound = true;
        it->second.set_response(response);
        break;
      }
    }

    if (!recipientFound)
      clients[fd].set_response(
          formatResponse(ERR_NOSUCHNICK, target + " :No such nick/channel"));
  }
}
