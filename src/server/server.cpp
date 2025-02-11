/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 04:27:32 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/28 04:27:32 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include <cstdlib>
#include <sstream>

static void DisplayPassInfo() {
  std::cout << formatServerMessage("ERROR", "Invalid password requirements:")
            << std::endl;
  std::cout << "- At least 8 characters" << std::endl;
  std::cout << "- At least 1 lowercase letter (a-z)" << std::endl;
  std::cout << "- At least 1 uppercase letter (A-Z)" << std::endl;
  std::cout << "- At least 1 digit (0-9)" << std::endl;
  std::cout << "- At least 1 special character" << std::endl;
}

void CoreServer::create_socket() {
  std::cout << formatServerMessage("DEBUG", "Creating socket...") << std::endl;

  ServData._socket = socket(AF_INET, SOCK_STREAM, 0);
  if (ServData._socket < 0) {
    std::cerr << formatServerMessage("ERROR", "Socket creation failed")
              << std::endl;
    exit(1);
  }
  std::ostringstream oss;
  oss << "Socket created: " << ServData._socket;
  std::cout << formatServerMessage("DEBUG", oss.str()) << std::endl;
  int flags = fcntl(ServData._socket, F_GETFL, 0);
  // if (flags < 0) {
  //     std::cerr << formatServerMessage("ERROR", "Fcntl failed") << std::endl;
  //     exit(1);
  // }
  fcntl(ServData._socket, F_SETFL, flags | O_NONBLOCK);
  int optval = 1;
  // if ()
  setsockopt(ServData._socket, SOL_SOCKET, SO_REUSEADDR, &optval,
             sizeof(optval));
  ServData.ServAddr.sin_family = AF_INET;
  ServData.ServAddr.sin_port = htons(ServData.Port);
  ServData.ServAddr.sin_addr.s_addr = INADDR_ANY;
  struct pollfd _fd;

  _fd.fd = ServData._socket;
  _fd.events = POLLIN;
  fds.push_back(_fd);
  std::cout << formatServerMessage("INFO", "Server Socket setup complete")
            << std::endl;
}

void CoreServer::start_listening() {

  if (bind(ServData._socket, (struct sockaddr *)&ServData.ServAddr,
           sizeof(ServData.ServAddr)) < 0) {
    std::cout << formatServerMessage("ERROR", "Binding failed") << std::endl;
    exit(1);
  }
  std::cout << formatServerMessage("DEBUG", "Starting to listen...")
            << std::endl;

  if (listen(ServData._socket, 5) < 0) {
    std::cout << formatServerMessage("ERROR", "Listen failed") << std::endl;
    exit(1);
  }
  std::ostringstream oss;
  oss << "Server listening on port: " << ServData.Port;
  std::cout << formatServerMessage("DEBUG", oss.str()) << std::endl;
}

void CoreServer::start_server() {
  std::cout << formatServerMessage("INFO", "Server main loop starting...")
            << std::endl;

  while (1) {

    int ret = poll(&fds[0], fds.size(), -1);
    if (ret < 0) {
      std::cout << formatServerMessage("FATAL", "Poll failed, Server Shutdown")
                << std::endl;
      break;
    }
    for (size_t i = 0; i < fds.size(); i++) {

      if (fds[i].revents & POLLIN) {
        if (fds[i].fd == ServData._socket)
          WelcomeClient();
        else
          ReadEvent(fds[i].fd);
        TickCycle();
        displayChannelTable();
      }
    }
  }
}

CoreServer::CoreServer(std::string port, std::string password) {

  if (!IsValidPort(port, ServData.Port)) {
    printServerMessage("ERROR", "Invalid port number");
    exit(1);
  }
  if (!IsValidPass(password, ServData.Passwd)) {
    printServerMessage("ERROR", "Invalid password requirements");
    DisplayPassInfo();
    exit(1);
  }

  printServerMessage("INFO", "CONFIGURATION: ");
  printServerMessage("INFO", "PORT: : " + numberToString(ServData.Port));
  printServerMessage("INFO", "PID: " + numberToString(getpid()));
  // print configuration if conf file used or defaults

  _serverStats.tickRate = TICK_RATE;
  _serverStats.tick = 0;
  _serverStats.uptime = 0;
  _serverStats.totalClients = 0;
  _serverStats.totalChannels = 0;
  _serverStats.totalMessages = 0;

  // _serverStats.totalCommands = 0;

  // mandatory commands
  commands[PASS] = &CoreServer::cmdPass;
  commands[NICK] = &CoreServer::cmdNick;
  commands[USER] = &CoreServer::cmdUser;
  commands[JOIN] = &CoreServer::cmdJoin;
  commands[MODE] = &CoreServer::cmdMode;
  commands[KICK] = &CoreServer::cmdKick;
  commands[INVITE] = &CoreServer::cmdInvite;
  commands[PART] = &CoreServer::cmdPart;
  commands[CAP] = &CoreServer::cmdCap; // only CAP LS and CAP END
  commands[PRIVMSG] = &CoreServer::cmdPrivmsg;
  // all the commands need to be guarded by a connection check.
  commands[PING] = &CoreServer::cmdPing;
  commands[QUIT] = &CoreServer::cmdQuit;
  commands[TOPIC] = &CoreServer::cmdTopic;
  commands["LIST"] = &CoreServer::cmdList;

  create_socket();
  start_listening();
  start_server();
}
