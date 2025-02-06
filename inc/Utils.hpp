#ifndef UTILS_HPP
#define UTILS_HPP

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
#define RESET   "\033[0m"


#define CRLF "\r\n"
#define HOST "morpheus-server.ddns.net"
#define SERVER_NAME ":WeUseArch "

#ifndef TICK_RATE
#   define TICK_RATE 1
#endif

// Global functions

std::string formatServerMessage(const std::string& type, const std::string& message);
void    formatArg(std::string &msg);
std::string numberToString(int value);





#endif
