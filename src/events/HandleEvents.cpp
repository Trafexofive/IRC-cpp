/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */ /*   HandleEvents.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 10:44:02 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 10:44:02 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

#include <algorithm>
#include <cstdio>
#include <netdb.h>
#include <sstream>

class FdRemovePredicate {
private:
  int fd_to_remove;

public:
  FdRemovePredicate(int fd) : fd_to_remove(fd) {}
  bool operator()(const struct pollfd &pfd) const {
    return pfd.fd == fd_to_remove;
  }
};

// Method to welcome a new client
void CoreServer::WelcomeClient() {

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  int fd_c =
      accept(ServData._socket, (struct sockaddr *)&client_addr, &client_len);

  if (fd_c < 0) {
    std::cout << formatServerMessage("WARNING", "Failure connecting client")
              << std::endl;
    return;
  }

  std::ostringstream oss;

  clients[fd_c] = Client(fd_c, client_addr);

  _serverStats.totalClients++;

  printServerMessage("INFO", "Client connected @fd: " + numberToString(fd_c));
  printServerMessage("INFO", "Total clients: " +
                                 numberToString(_serverStats.totalClients));

  char host[NI_MAXHOST];
  char service[NI_MAXSERV];
  getnameinfo((struct sockaddr *)&clients[fd_c].getClientInfos(),
              sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV,
              NI_NUMERICHOST | NI_NUMERICSERV);

  struct pollfd _fd;
  _fd.fd = fd_c;
  _fd.events = POLLIN;
  fds.push_back(_fd);
}

void CoreServer::handleCommand(int fd, const std::string &line) {
  try {
    std::istringstream iss(line);
    std::vector<std::string> args;
    std::string arg;

    while (iss >> arg) {
      args.push_back(arg);
    }

    if (args.empty()) {
      std::cout << formatServerMessage("DEBUG", "Empty command, Omitting")
                << std::endl;
      return;
    }

    std::string command = args[0];
    std::string::iterator it;
    for (it = command.begin(); it != command.end(); ++it) {
      *it = toupper(*it);
    }

    //tmp
    if (clients[fd].isStatus(STATUS::UNKNOWN) && command == CAP)
        printServerMessage("INFO", "Client CAP request @fd: " + numberToString(fd) + " - " + line);
    else if (clients[fd].isStatus(STATUS::UNKNOWN) && command == "CAP LS")
        printServerMessage("INFO", "Client NICK request @fd: " + numberToString(fd) + " - " + line);
    else if (clients[fd].isStatus(STATUS::UNKNOWN) && command != "PASS" ){
      std::cout << formatServerMessage("SYSTEM",
                                       "YUP Client not authenticated @fd: ")
                << clients[fd].getFd() << std::endl;
      clients[fd].setResponse(
          formatResponse(ERR_PASSWDMISMATCH, ":Password required"));
      WriteEvent(fd);
      return;
    }

    std::cout << formatServerMessage("DEBUG", "Processing command: " + command)
              << std::endl;

    std::map<std::string, CommandHandler>::iterator cmdIt =
        commands.find(command);
    if (cmdIt != commands.end()) {
      try {

        (this->*cmdIt->second)(fd, args);
        WriteEvent(fd);

      } catch (const std::exception &e) {
        std::cerr << formatServerMessage(
                         "ERROR", std::string("Failed to execute command: ") +
                                      command + " - " + e.what())
                  << std::endl;
      }
    } else {
      std::cerr << formatServerMessage(
                       "WARNING", std::string("Unknown command: ") + command)
                << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << formatServerMessage(
                     "ERROR",
                     std::string("Failed to process command line: ") + e.what())
              << std::endl;
  }
}

// Method to send a response to the client
void CoreServer::WriteEvent(int fd) {
    if (isClientDisconnected(fd)) // could cause issues
        return;
  if (!clients[fd].getResponse().empty()) {
    const std::string &response = clients[fd].getResponse();
    printServerMessage("SERVER", clients[fd].getTarget() + " >>" + response);

    ssize_t written = send(fd, response.c_str(), response.length(), 0);

    clients[fd].clearResponse();

    if (written < 0) {
      std::cout << formatServerMessage("WARNING", "Writing error") << std::endl;
    } else {
      clients[fd].clearResponse();
    }
  }
}

void CoreServer::purgeDisconnectedClients() {
  std::map<int, Client>::iterator it = clients.begin();
  std::map<int, Client>::iterator tmp;

  while (tmp != clients.end()) {
    tmp = it;
    ++tmp;
    if (it->second.isStatus(STATUS::DISCONNECTED)) {
        printServerMessage("DEBUG", "Purging disconnected client @fd: " + numberToString(it->first));

      clients.erase(it);
    }
    it = tmp;
  }
}

void CoreServer::handleDisconnect(int fd) {

  unsubFromChannels(fd);
  _serverStats.totalClients--;

  std::vector<struct pollfd>::iterator new_end =
      std::remove_if(fds.begin(), fds.end(), FdRemovePredicate(fd));
  fds.erase(new_end, fds.end());
  disableClient(fd);
}

// Method to read data from the client
void CoreServer::ReadEvent(int fd) {
  char buffer[1024];
  std::memset(buffer, 0, sizeof(buffer));

  int dataRead = read(fd, buffer, sizeof(buffer) - 1);

  if (dataRead <= 0) {
    std::ostringstream oss;
    oss << "Closing connection FD: " << fd;
    std::cout << formatServerMessage("INFO", oss.str()) << std::endl;

    
    handleDisconnect(fd);
    return;
  }

  printServerMessage("CLIENT", clients[fd].getTarget() + " " + buffer);
  buffer[dataRead] = '\0';
  std::string input(buffer);
  std::istringstream iss(input);
  std::string line;

  while (std::getline(iss, line)) {
    // Remove \r if present
    if (!line.empty() && line[line.length() - 1] == '\r')
      line.erase(line.length() - 1);

    if (line.empty())
      continue;

    handleCommand(fd, line);
  }
}
