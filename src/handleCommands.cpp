#include "core_server.hpp"

void CoreServer::handleCommands(int fd, std::string _cmd)
{
    std::istringstream iss(_cmd);
    std::string command;
    std::string args;
    
    // Extract the command (first word)
    iss >> command;
    
    // Get the rest as arguments
    std::getline(iss, args);
    if (!args.empty() && args[0] == ' ')
        args = args.substr(1);

    // Convert command to uppercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    
    // Find and execute the command
    std::map<std::string, CommandHandler>::iterator it = commands.find(command);
    if (it != commands.end())
    {
        // Execute the command handler
        (this->*(it->second))(fd, args);
    }
    else
    {
        // Command not found
        std::string error = "Unknown command: " + command + "\r\n";
        send(fd, error.c_str(), error.length(), 0);
    }
}