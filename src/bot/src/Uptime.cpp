/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uptime.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 08:25:01 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/26 08:25:01 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"

bool Bot::checkRuntime(time_t time) {
  if (_uptime == time)
    return true;
  return false;
}

void Bot::initUptime() {
  std::thread t([this]() {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(_tickrate));
      _uptime++;
      checkReminders();
      // runChecks();
      if (checkRuntime(_runtime)) {
        break;
      }
    }
  });
  t.detach();
}

time_t Bot::getUptime() { return _uptime; }

void Bot::DisplayUptime(const std::string &id) {

  if (isUser(id) && isMaster(id)) {
    messageUser(id, "Bot: " + _env.getNickname() + " has been up for " +
                        std::to_string(getUptime()) + " seconds");
  } else if (isChannel(id)) { // check if channel is in the channel list
    messageChannel(id, "Bot: " + _env.getNickname() + " has been up for " +
                           std::to_string(getUptime()) + " seconds");
  } else {
    messageChannel(id, "You do not have permission to use this command");
  }
}
