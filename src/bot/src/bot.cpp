/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 20:26:20 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/19 20:26:20 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"
#include "../inc/Reminder.hpp"
#include "../inc/env.hpp"
#include <unistd.h>

Bot::Bot() : _env(Env()), _uptime(0), _tickrate(1000), _debug(false), _runtime(0) {}


Bot::Bot(const Bot &other) { *this = other; }

Bot &Bot::operator=(const Bot &other) {
    _env = other._env;
    _uptime = other._uptime;
    _tickrate = other._tickrate;
    _reminders = other._reminders;
    _masters = other._masters;
    _debug = other._debug;
    _runtime = other._runtime;
    return *this;
}

Bot::~Bot() { disconnectFromServer(); }



// ################################## CORE

bool Bot::processServerResponse() {
  char buffer[1024];
  int bytesRead = recv(_env.getClientFdSocket()  , buffer, sizeof(buffer) - 1, 0);

  if (bytesRead > 0) {
    buffer[bytesRead] = '\0';
    translateServerResponse(buffer);

  } else if (bytesRead == 0) {
    std::cout << "Info: Server closed the connection: exiting ..." << std::endl;
    disconnectFromServer();
    return false;
  } else {
    std::cerr << "Error: Error reading from server: exiting ..." << std::endl;
    disconnectFromServer();
    return false;
  }
  return true;
}

void Bot::translateServerResponse(const std::string &response) {

  std::string user = response.substr(1, response.find("!") - 1);
  user = user.substr(0, user.find("@"));
  std::string channel = response.substr(response.find(":") + 1);
  channel = channel.substr(0, channel.find(" "));

  if (_debug) {
    std::cout << "User: " << user << " Channel: " << channel << std::endl;
  }

  if (isCommand(response)) {
    processUserCommand(response);
  } else if (response.find("PING") != std::string::npos) {
    std::string pong = "PONG " + response.substr(5) + "\r\n";
    sendMessageToServer(pong);
  } else if (response.find("JOIN") != std::string::npos && isChannel(channel)) {
    if (_debug) {
      std::cout << "User: " << user << " joined channel: " << channel
                << std::endl;
    }
  } else if (response.find("PRIVMSG") != std::string::npos) {
    handlePrivmsg(response);
  } else {
    if (_debug) {
      std::cout << response << std::endl;
    }
  }
}
