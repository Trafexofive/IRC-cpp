#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
// System includes
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
#include <string>
#include <sstream>
#include <ctime>

// Color definitions
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

#define BOLD    "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define RESET   "\033[0m"

#define CRLF "\r\n"

const std::string HOST = "morpheus-server.ddns.net";
const std::string SERVER_NAME = ":WeUseArch.localhost ";

#ifndef TICK_RATE
#   define TICK_RATE 0
#endif



typedef enum {
    INFO,
    WARNING,
    DEBUG,
    ERROR,
    VERBOSE
} LEVEL;

// Server data structure
struct ServerData {
    std::string Passwd;
    int _socket;
    int _poll;
    int Port;
    std::string Host;

    // std::string Motd;
    // std::string Welcome;
    
    std::string ServerName;

    LEVEL _infoLevel;

    struct sockaddr_in ServAddr;
    
    ServerData() : Passwd("password"), _socket(-1), _poll(-1), Port(6667), Host("morpheus-server.ddns.net"), ServerName("WeUseArch"), _infoLevel(INFO) {
        memset(&ServAddr, 0, sizeof(ServAddr));
    }
    
};
// Global functions

std::string formatServerMessage(const std::string& type, const std::string& message);
void    formatArg(std::string &msg);
std::string numberToString(int value);





#endif
