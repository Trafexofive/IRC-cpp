#ifndef UTILS_HPP
#define UTILS_HPP

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

// Declare the formatServerMessage function
std::string formatServerMessage(const std::string& type, const std::string& message);

#endif
