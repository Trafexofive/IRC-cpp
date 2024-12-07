/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 02:03:57 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/25 02:03:57 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"

bool Bot::isCommand(const std::string &response) {
  if (response.find("!cmd") != std::string::npos)
    return true;
  return false;
}

std::string Bot::getArgIndex(int index, const std::string &args) {
  std::string arg;
  int count = 0;

  for (int i = 0; args[i] != '\0'; i++) {
    if (args[i] == ' ') {
      count++;
    } else if (count == index) {
      arg += args[i];
    }
  }
  return arg;
}

std::string Bot::getCommand(const std::string &response) {
  std::string command = response.substr(response.find("!cmd") + 5);
  command = command.substr(0, command.find(" "));
  // command = command.substr(0, command.size() - 1);
  return command;
}

void Bot::processUserCommand(const std::string &response) {

  std::string user = response.substr(1, response.find("!") - 1);
  user = user.substr(0, user.find("@"));
  std::string args = getCommandArgs(response);
  std::string command = getCommand(response);
  std::cout << "Command: " << command << std::endl;


  if (command.find("help") != std::string::npos)
    help(user);
  else if (command.find("whisper") != std::string::npos)
    whisperUser(user, getArgIndex(1, args));
  else if (command.find("info") != std::string::npos)
    info(user);
  else if (command.find("uptime") != std::string::npos)
    DisplayUptime(user);
  else if (command.find("remind") != std::string::npos)
    addReminderUser(getArgIndex(1, args), getArgIndex(2, args),
                    std::stoi(getArgIndex(3, args)), user);
}

std::string Bot::getCommandArgs(const std::string &response) {
  // find /cmd then add first word after /cmd to vector

  std::string args;

  if (response.find("!cmd") != std::string::npos) {
    args = response.substr(response.find("!cmd") + 5);
  }
  return args;
}

int Bot::commandArgsCount(const std::string &command) {
  int count = 0;

  for (int i = 0; command[i] != '\0'; i++) {
    if (command[i] == ' ')
      count++;
  }

  return count;
}

bool Bot::isChannel(const std::string &channel) {
  if (channel[0] == '#')
    return true;
  return false;
}

bool Bot::isUser(const std::string &user) {
  if (user[0] == '@')
    return true;
  return false;
}
