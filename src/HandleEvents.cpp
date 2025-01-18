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

#include "../inc/Server.hpp"
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
  oss << "Client accepted FD: " << fd_c;
  std::cout << formatServerMessage("INFO", oss.str()) << std::endl;

  clients[fd_c] = Client(fd_c, client_addr);

  std::cout << formatServerMessage("INFO", "Client connected") << std::endl;

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

static void printTokens(const std::vector<std::string> &tokens) {
  std::cout << formatServerMessage("DEBUG", "Tokens: ") << std::endl;
  for (std::vector<std::string>::const_iterator it = tokens.begin();
       it != tokens.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;
}

void CoreServer::handleCommand(int fd, const std::string &line) {
  try {
    // Parse command line into tokens
    std::istringstream iss(line);
    Client &client = clients[fd];
    std::vector<std::string> args;
    std::string arg;

    while (iss >> arg) {
      args.push_back(arg);
    }
    if (args.empty()) {
      return;
    }
    if (args[0] == "QUIT") {
      client.setResponse("Goodbye!\r\n");
      WriteEvent(fd);
      return;
    }
    if (client.getAuth())
        client.constructSource();

    // debug
    // printTokens(args);

    // Convert command to uppercase
    std::string command = args[0];
    std::string::iterator it;
    for (it = command.begin(); it != command.end(); ++it) {
      *it = toupper(*it);
    }

    std::cout << formatServerMessage("DEBUG", "Processing command: " + command)
              << std::endl;

    // Execute command if it exists

    std::map<std::string, CommandHandler>::iterator cmdIt =
        commands.find(command);
    if (cmdIt != commands.end()) {
      try {
        (this->*cmdIt->second)(fd, args);
        WriteEvent(fd); // Send response immediately after command
        client.printClientInfo();
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
  if (!clients[fd].getResponse().empty()) {
    const std::string &response = clients[fd].getResponse();
    std::cout << formatServerMessage("SERVER", response)
              << std::endl;

    ssize_t written = send(fd, response.c_str(), response.length(), 0);
    clients[fd].clearResponse();

    if (written < 0) {
      std::cout << formatServerMessage("WARNING", "Writing error") << std::endl;
    } else {
      clients[fd].clearResponse();
    }
  }
}

// Method to read data from the client
void CoreServer::ReadEvent(int fd) {
  char buffer[1024];
  std::memset(buffer, 0, sizeof(buffer));

  int readed = read(fd, buffer, sizeof(buffer) - 1);

  if (readed <= 0) {
    std::ostringstream oss;
    oss << "Closing connection FD: " << fd;
    std::cout << formatServerMessage("INFO", oss.str()) << std::endl;

    std::vector<struct pollfd>::iterator new_end =
        std::remove_if(fds.begin(), fds.end(), FdRemovePredicate(fd));
    fds.erase(new_end, fds.end());
    close(fd);
    clients.erase(fd);
    return;
  }

  buffer[readed] = '\0';
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
