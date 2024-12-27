#include "../inc/Utils.hpp"

std::string formatServerMessage(const std::string& type, const std::string& message)
{
    std::ostringstream oss;
    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    oss << "[" << timestamp << "] ";
    
    if (type == "DEBUG")
        oss << YELLOW << "DEBUG :: " << RESET;
    else if (type == "ERROR")
        oss << RED << "ERROR :: " << RESET;
    else if (type == "CLIENT")
        oss << CYAN << "CLIENT >> " << RESET;
    else if (type == "SERVER")
        oss << MAGENTA << "SERVER << " << RESET;
    
    oss << message;
    return oss.str();
}
