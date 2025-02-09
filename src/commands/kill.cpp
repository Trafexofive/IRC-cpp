#include "../inc/Server.hpp"

void CoreServer::cmdKill(int fd, std::vector<std::string> &args)
{
    if (clients[fd].isDisconnected())
    {
        std::string msg = formatResponse(ERR_NOSUCHNICK, "No such nick/channel");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    if (clients[fd].getOperator() == false)
    {
        std::string msg = formatResponse(ERR_NOPRIVILEGES, "Permission Denied- You're not an IRC operator");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    if (args.size() < 2 || args[1].at(0) != ':')
    {
        std::string msg = formatResponse(ERR_NEEDMOREPARAMS, "Not enough parameters");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    std::string target = args[1];
    std::string message;
    for (size_t i = 2; i < args.size(); i++)
    {
        message += args[i];
        if (i + 1 < args.size())
        {
            message += " ";
        }
    }
    std::map<int, Client>::iterator it = clients.begin();
    for (; it != clients.end(); ++it)
    {
        if (it->second.getNickName() == args[1])
        {
            std::string msg = formatResponseSource(clients[fd].getTarget(), "KILL", args[1].substr(1) + " :Killed by operator reason: " + message);
            send(it->first, msg.c_str(), msg.size(), 0);
            handleDisconnect(it->first);
            return;
        }
    }
    std::string msg = formatResponse(ERR_NOSUCHNICK, "No such nick/channel");
    send(fd, msg.c_str(), msg.size(), 0);
}