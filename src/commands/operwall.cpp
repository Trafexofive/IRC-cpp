#include "../inc/Server.hpp"

void    CoreServer::cmdoperwall(int fd, std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::string msg = formatResponse(ERR_NEEDMOREPARAMS, "Not enough parameters");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    if (!clients[fd].getOperator())
    {
        std::string msg = formatResponse(ERR_NOPRIVILEGES, "Permission Denied- You're not an IRC operator");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    std::string message;
    for (size_t i = 1; i < args.size(); i++)
    {
        message += args[i];
        if (i + 1 < args.size())
        {
            message += " ";
        }
    }
    // std::vector<Client *>::iterator it = getOperators().begin();
    // for (;it != getOperators().end();++it)
    // {
    //     std::string msg = formatResponseSource("NOTICE", "@#" ,message);
    //     send((*it)->getFd(), msg.c_str(), msg.size(), 0);
    // }
    std::map<int, Client *> operators = getOperators();
    for (std::map<int, Client *>::iterator it = operators.begin(); it != operators.end(); ++it)
    {
        std::cout << "operwall: " << message << std::endl;
        std::string msg = formatResponseSource("NOTICE", "@#" ,message);
        send(it->first, msg.c_str(), msg.size(), 0);
    }
}