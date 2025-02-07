#include "../inc/Server.hpp"

void    CoreServer::setOperator(Client &client)
{
    if (client.isDisconnected())
    {
        std::string msg = formatResponse(ERR_NOSUCHNICK, "No such nick/channel");
        std::cout << formatServerMessage("ERROR", "User not found") << std::endl;
        send(client.getFd(), msg.c_str(), msg.size(), 0);
    }
    client.beoper();
    operators.push_back(&client);
}

void    CoreServer::cmdoper(int fd, std::vector<std::string> &args)
{
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
    if (args[1] != ServData.Opuser || passwd != ServData.Oppass)
    {
        std::string msg = formatResponse(ERR_PASSWDMISMATCH, "Password required");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    clients[fd].setOperator(true);
    // _operator.push_back(&clients[fd]);
    std::string msg = formatResponse(RPL_YOUREOPER, "You are now an IRC operator");
    send(fd, msg.c_str(), msg.size(), 0);
}