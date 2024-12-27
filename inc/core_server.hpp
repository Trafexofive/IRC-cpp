#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

#include "./Client.hpp"
#include "./requestMacros.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "./Client.hpp"
#include "./Utils.hpp" // Add this include
#include "./requestMacros.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

// Remove the formatServerMessage function definition from here

// Rest of the core_server.hpp remains the same
// Color definitions
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

class CoreServer;
typedef void (CoreServer::*CommandHandler)(int, std::vector<std::string> &);

// Custom remove_if function for C++98
template <typename Iterator, typename Predicate>
Iterator custom_remove_if(Iterator first, Iterator last, Predicate pred) {
  Iterator result = first;
  for (; first != last; ++first) {
    if (!pred(*first)) {
      if (result != first)
        *result = *first;
      ++result;
    }
  }
  return result;
}

struct ServerData {
  std::string Passwd;
  int _socket;
  int _poll;
  int Port;
  struct sockaddr_in ServAddr;

  ServerData() : _socket(-1), _poll(-1), Port(-1) {
    std::memset(&ServAddr, 0, sizeof(ServAddr));
  }
};

// Predicate for removing file descriptors
struct FdPredicate {
  int fd_to_remove;
  explicit FdPredicate(int fd) : fd_to_remove(fd) {}
  bool operator()(const struct pollfd &pfd) const {
    return pfd.fd == fd_to_remove;
  }
};

class CoreServer {
private:
  ServerData ServData;
  std::map<int, _client> clients;
  std::vector<struct pollfd> fds;
  std::vector<channel> channels;
  std::map<std::string, CommandHandler> commands;

  // Socket and server initialization
  void create_socket();
  void start_listening();
  void start_server();

  // Command handlers
  void cmdNick(int fd, std::vector<std::string> &args);
  void cmdUser(int fd, std::vector<std::string> &args);
  void cmdPass(int fd, std::vector<std::string> &args);
  void cmdJoin(int fd, std::vector<std::string> &args);
  void cmdPrivmsg(int fd, std::vector<std::string> &args);

  // Helper methods
  void disconnectClient(int fd);
  void broadcastToChannel(const std::string &channelName,
                          const std::string &message,
                          const std::string &except_nick = "");

public:
  // Constructor and destructor
  CoreServer(std::string port, std::string password);
  ~CoreServer() {
    // Cleanup connections
    for (std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end();
         ++it) {
      if (it->fd >= 0)
        close(it->fd);
    }
  }

  // Client handling methods
  void WelcomeClient();
  void WriteEvent(int fd);
  void ReadEvent(int fd);
  void handleCommands(int fd, const std::string &_cmd);

  // Utility methods
  void removeFd(int fd) {
    fds.erase(custom_remove_if(fds.begin(), fds.end(), FdPredicate(fd)),
              fds.end());
  }

  // Getters
  const std::map<int, _client> &getClients() const { return clients; }
  const std::vector<channel> &getChannels() const { return channels; }
};

// Non-member functions for validation
bool IsValidPort(std::string port, int &_tport);
bool IsValidPass(std::string _pass, std::string &passwd);

#endif // CORE_SERVER_HPP
