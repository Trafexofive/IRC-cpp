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

static void displayConnectionInfo(struct sockaddr_in client) {
  std::ostringstream oss;
  oss << "Connection from: " << inet_ntoa(client.sin_addr) << ":"
      << ntohs(client.sin_port);
  std::cout << formatServerMessage("INFO", oss.str()) << std::endl;
}

static void displayActiveConnections(std::vector<struct pollfd> fds) {
  std::ostringstream oss;
  oss << "Active connections: " << fds.size();
  std::cout << formatServerMessage("DEBUG", oss.str()) << std::endl;
}
void CoreServer::start_listening() {
  std::cout << formatServerMessage("DEBUG", "Binding socket...") << std::endl;
  if (bind(ServData._socket, (struct sockaddr *)&ServData.ServAddr,
           sizeof(ServData.ServAddr)) < 0) {
    std::cout << formatServerMessage("ERROR", "Binding failed") << std::endl;
    exit(1);
  }
  std::cout << formatServerMessage("DEBUG", "Starting to listen...")
            << std::endl;
  if (listen(ServData._socket, 5) < 0) {
    std::cerr << formatServerMessage("ERROR", "Listen failed") << std::endl;
    exit(1);
  }
  std::ostringstream oss;
  oss << "Server listening on port: " << ServData.Port;
  std::cout << formatServerMessage("DEBUG", oss.str()) << std::endl;
}
void CoreServer::start_server() {
  std::cout << formatServerMessage("DEBUG", "Server main loop starting...")
            << std::endl;

  while (1) {
    int ret = poll(&fds[0], fds.size(), -1);
    if (ret < 0) {
      std::cout << formatServerMessage("FATAL", "Poll failed") << std::endl;
      break;
    }
    displayActiveConnections(fds);
    for (size_t i = 0; i < fds.size(); i++) {
      if (fds[i].revents & POLLIN) {
        if (fds[i].fd == ServData._socket)
          WelcomeClient();
        else
          ReadEvent(fds[i].fd);
      }
    }
  }
}

static void DisplayPassInfo() {
  std::cout << formatServerMessage("ERROR", "Invalid password requirements:")
            << std::endl;
  std::cout << "- At least 8 characters" << std::endl;
  std::cout << "- At least 1 lowercase letter (a-z)" << std::endl;
  std::cout << "- At least 1 uppercase letter (A-Z)" << std::endl;
  std::cout << "- At least 1 digit (0-9)" << std::endl;
  std::cout << "- At least 1 special character" << std::endl;
}
CoreServer::CoreServer(std::string port, std::string password) {
  std::cout << formatServerMessage("INFO", "Constructing Server Class ...")
            << std::endl;
  if (!IsValidPort(port, ServData.Port)) {
    std::cerr << formatServerMessage("ERROR", "Invalid port") << std::endl;
    exit(1);
  }
  if (!IsValidPass(password, ServData.Passwd)) {
    std::cerr << formatServerMessage("ERROR", "Invalid Password") << std::endl;
    DisplayPassInfo();
    exit(1);
  }
  std::cout << formatServerMessage("DEBUG", "Registering commands...")
            << std::endl;
  // only one allowed before auth
  commands[PASS] = &CoreServer::cmdPass;
  commands[NICK] = &CoreServer::cmdNick;
  commands[USER] = &CoreServer::cmdUser;
  commands[CAP] = &CoreServer::cmdCap;
  commands[JOIN] = &CoreServer::cmdJoin;
  commands[PRIVMSG] = &CoreServer::cmdPrivmsg;
  commands[PING] = &CoreServer::cmdPing;
  commands[PART] = &CoreServer::cmdPart;
  commands[QUIT] = &CoreServer::cmdQuit;
  // commands[CMD_LIST] = &CoreServer::cmdList;
  create_socket();
  start_listening();
  start_server();
}
