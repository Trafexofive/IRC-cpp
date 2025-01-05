/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 21:01:07 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/15 21:01:07 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include "../inc/Reminder.hpp"
#include "../inc/env.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Bot {
private:
  Env _env;

  time_t _uptime;
  int _tickrate;

  std::vector<Reminder> _reminders;
  std::string _masters;
  bool _debug;
  time_t _runtime;

public:
  Bot();
  Bot(const Bot &other);
  Bot &operator=(const Bot &other);
  ~Bot();

  typedef std::vector<std::string>::iterator VecIt;
  typedef std::vector<std::string>::const_iterator VecItconst;

  // auth methods
  bool connectToServer();
  void disconnectFromServer();

  // core
  bool coreLoop();
  // commands / messages
  void sendMessageToServer(const std::string &message);
  bool processServerResponse();
  void sendToAllChannels(const std::string &message);

  void joinChannel(const std::string &channelName);
  void joinChannel();
  void leaveChannel(const std::string &channelName);

  // auth
  bool registerBot();
  // command handler
  void handleCommand(const std::string &line);
  void handlePrivmsg(const std::string &response);
  // Display
  void DisplayWelcome();
  void DisplayHelp();
  void DisplayDate();
  void DisplayUptime(const std::string &id);
  // channels
  void listChannels();

  void setChannel(const std::string &channel);
  void removeChannel(const std::string &channel);
  void messageChannel(const std::string &message);
  void messageChannel(const std::string &channelName,
                      const std::string &message);
  void messageUser(const std::string &user, const std::string &message);

  void pingUser(const std::string &user);
  void pingServer();

  // Parse & config

  bool loadChannels(const std::string &line);
  void parseConfigFile(const std::string &filename);
  void parseArgs(int argc, char **argv);
  std::string loadCommands(const std::string &line);
  void handleServerAddr(const std::string &line);
  bool loadMasters(const std::string &line);

  bool minimumAllowedArgs();

  void translateServerResponse(const std::string &response);

  void processCommand(const std::string &command);
  void processChannelCommand(const std::string &response);
  void processUserCommand(const std::string &response);
  void checkResponse(const std::string &response);
  bool isCommand(const std::string &line);
  int commandArgsCount(const std::string &args);
  std::string getCommandArgs(const std::string &response);
  std::string getArgIndex(int index, const std::string &args);
  std::string getCommand(const std::string &response);

  // REMINDER
  void initReminders();
  time_t getUptime();
  void initUptime();
  void addReminderChannel(const std::string &title, const std::string &message,
                          time_t reminderTime, const std::string &channel);
  void addReminderUser(const std::string &title, const std::string &message,
                       time_t reminderTime, const std::string &nickname);
  void checkReminders();
  void SendReminder(Reminder &reminder);
  bool checkRuntime(time_t time);

  bool isChannel(const std::string &channel);
  bool isMaster(const std::string &user);
  bool isUser(const std::string &user);
  bool inChannels(const std::string &channel);
  bool isMasterChannel(const std::string &channel);

  void whisperUser(const std::string &user, const std::string &message);
  void whisperChannel(const std::string &channel, const std::string &message);

  void help(const std::string &id);
  void info(const std::string &id);
  void DisplayBotInfo();

  bool handleConfigLine(const std::string &line);

  void unsubFromChannel(const std::string &channelName);
};

#endif
