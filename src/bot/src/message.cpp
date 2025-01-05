/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 23:09:51 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/24 23:09:51 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"

// #################################### GETTERS AND SETTERS

// #################################### MESSAGE SPECIFIC METHODS

void Bot::messageChannel(const std::string &channelName,
                         const std::string &message) {
  sendMessageToServer("PRIVMSG " + channelName + " :" + message + "\r\n");
}

void Bot::messageChannel(const std::string &message) {
  // overload for messaging all user registered to bot.

  for (auto it = _env.getChannels().begin(); it != _env.getChannels().end();
       it++) {
    messageChannel(*it, message);
  }
}

void Bot::messageUser(const std::string &user, const std::string &message) {
  sendMessageToServer("PRIVMSG " + user + " :" + message + "\r\n");
  if (_debug) {
    std::cout << message << std::endl;
  }
}

void Bot::handlePrivmsg(const std::string &response) {
  if (response.find("#") != std::string::npos) {
    std::string channel = response.substr(
        response.find("#"), response.find(" :") - response.find("#"));
    std::string message = response.substr(response.find(" :") + 2);
    std::string user = response.substr(1, response.find("!") - 1);
    std::cout << "Channel: " << channel << " User: " << user
              << " Message: " << message << std::endl;
  } else {
    std::string user = response.substr(1, response.find("!") - 1);
    std::string message = response.substr(response.find(" :") + 2);
    std::cout << "User: " << user << " Message: " << message << std::endl;
  }
}

void Bot::pingUser(const std::string &user) {
  // remove @ from user
  if (user.empty()) {
    return;

  } else if (user[0] == '@') {
    sendMessageToServer("PING" + user.substr(1) + "\r\n");
  }
}

void Bot::pingServer() {
  sendMessageToServer("PING " + _env.getServerAddress() + "\r\n");
}

void Bot::sendMessageToServer(const std::string &message) {
  if (_debug) {
    std::cout << "Sending message: " << message << std::endl;
  }
  if (send(_env.getClientFdSocket(), message.c_str(), message.size(), 0) < 0) {
    std::cerr << "Error: Can't send message to server" << std::endl;
  }
}

void Bot::whisperUser(const std::string &user, const std::string &message) {
  messageUser(user, "Whisper from unknown user ->" + message);
}

void Bot::whisperChannel(const std::string &channel,
                         const std::string &message) {
  messageChannel(channel, "Whisper from unknown user ->" + message);
}
