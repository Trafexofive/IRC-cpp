#include "../inc/Server.hpp"

void    CoreServer::cmdoper(int fd, std::vector<std::string> &args)
{
    std::string user("admin");
    std::string pass("admin");
    if (args.size() < 3)
    {
        std::string msg = formatResponse(ERR_NEEDMOREPARAMS, "Not enough parameters");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    std::string passwd;
    for (size_t i = 1; i < args.size(); i++)
    {
        passwd += args[i];
        if (i + 1 < args.size())
        {
            passwd += " ";
        }
    }
    if (args[1] != user || passwd != pass)
    {
        std::string msg = formatResponse(ERR_PASSWDMISMATCH, ":Password required");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    clients[fd].setOperator(true);
    operators.push_back(&clients[fd]);
}