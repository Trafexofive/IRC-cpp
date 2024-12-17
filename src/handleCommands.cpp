#include "core_server.hpp"

// std::map<std::string , CommandHandler> commands;

void CoreServer::handleCommands(int fd, std::string _cmd)
{
    std::istringstream iss(_cmd);
    std::string command;
    std::vector<std::string> args;
    std::string _buff;
    std::string tmp;
    // std::string args;
    
    iss >> command;
    
    // std::getline(iss, args);
    // if (!args.empty() && args[0] == ' ')
    //     args = args.substr(1);
    bool t = false;
    while (1)
    {
        std::getline(iss,_buff);
        if (t == false)
        {
            iss >> command;
            t = true;
        }
        iss >> tmp;
        args.push_back(tmp);
        
    }
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    
    std::map<std::string, CommandHandler>::iterator it = commands.find(command);
    if (it != commands.end())
    {
        (this->*(it->second))(fd, args);
    }
    else
    {
        std::string error = "Unknown command: " + command + "\r\n";
        send(fd, error.c_str(), error.length(), 0);
    }
}