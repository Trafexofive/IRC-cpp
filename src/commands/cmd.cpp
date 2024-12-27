/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 04:05:32 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 04:05:32 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/core_server.hpp"
#include <sstream>

void CoreServer::handleCommands(int fd, const std::string& _cmd)
{
    std::vector<std::string> args;
    std::istringstream iss(_cmd);
    std::string token;
    
    while (iss >> token)
        args.push_back(token);
    
    if (args.empty())
        return;

    std::string cmd = args[0];
    std::map<std::string, CommandHandler>::iterator it = commands.find(cmd);
    
    if (it != commands.end())
        (this->*(it->second))(fd, args);
    else
        clients[fd].set_response(ERR_UNKNOWNCOMMAND);
}

void CoreServer::cmdPass(int fd, std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        clients[fd].set_response(FORMAT_RESPONSE("461", "PASS :Not enough parameters"));
        return;
    }
    
    if (args[1] != ServData.Passwd)
    {
        clients[fd].set_response(FORMAT_RESPONSE("464", ":Password incorrect"));
        return;
    }
    
    clients[fd].set_bool(true);
}

void CoreServer::cmdNick(int fd, std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        clients[fd].set_response(FORMAT_RESPONSE("461", "NICK :Not enough parameters"));
        return;
    }
    
    const std::string& nickname = args[1];
    
    // Check for invalid characters
    if (nickname.find_first_of(" ,*?!@") != std::string::npos)
    {
        clients[fd].set_response(FORMAT_RESPONSE("432", "* " + nickname + " :Erroneous nickname"));
        return;
    }
    
    // Check if nickname is already in use
    for (std::map<int, _client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.get_nick() == nickname)
        {
            clients[fd].set_response(ERR_NICKNAMEINUSE);
            return;
        }
    }

    std::string oldNick = clients[fd].get_nick();
    clients[fd].set_nick(nickname);
    
    if (oldNick.empty() && clients[fd].get_bool())
    {
        clients[fd].set_response(RPL_WELCOME);
        clients[fd].set_response(RPL_YOURHOST);
        clients[fd].set_response(RPL_CREATED);
        clients[fd].set_response(RPL_MYINFO);
    }
    else if (!oldNick.empty())
    {
        std::string nickChange = ":" + oldNick + " NICK :" + nickname + "\r\n";
        for (std::map<int, _client>::iterator it = clients.begin(); it != clients.end(); ++it)
            it->second.set_response(nickChange);
    }
}

void CoreServer::cmdUser(int fd, std::vector<std::string>& args)
{
    if (args.size() < 5)
    {
        clients[fd].set_response(FORMAT_RESPONSE("461", "USER :Not enough parameters"));
        return;
    }

    std::string username = args[1];
    std::string realname = args[4];
    
    clients[fd].set_user(username);
    if (clients[fd].get_bool() && !clients[fd].get_nick().empty())
    {
        clients[fd].set_response(RPL_WELCOME);
        clients[fd].set_response(RPL_YOURHOST);
        clients[fd].set_response(RPL_CREATED);
        clients[fd].set_response(RPL_MYINFO);
    }
}

void CoreServer::cmdJoin(int fd, std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        clients[fd].set_response(FORMAT_RESPONSE("461", "JOIN :Not enough parameters"));
        return;
    }
    
    std::string channelName = args[1];
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    // Find or create channel
    bool channelExists = false;
    for (std::vector<channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->getName() == channelName)
        {
            channelExists = true;
            it->addMember(clients[fd]);
            break;
        }
    }
    
    if (!channelExists)
    {
        channel newChannel(channelName);
        newChannel.addMember(clients[fd]);
        channels.push_back(newChannel);
    }

    // Notify all clients in channel
    std::string joinMsg = ":" + clients[fd].get_nick() + " JOIN " + channelName + "\r\n";
    for (std::map<int, _client>::iterator it = clients.begin(); it != clients.end(); ++it)
        it->second.set_response(joinMsg);

    // Send names list
    std::string names = "353 " + clients[fd].get_nick() + " = " + channelName + " :";
    for (std::vector<channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->getName() == channelName)
        {
            const std::vector<_client>& members = it->getMembers();
            for (std::vector<_client>::const_iterator mit = members.begin(); mit != members.end(); ++mit)
            {
                if (mit != members.begin())
                    names += " ";
                names += mit->get_nick();
            }
            break;
        }
    }
    clients[fd].set_response(names + "\r\n");
    clients[fd].set_response(RPL_ENDOFNAMES);
}

void CoreServer::cmdPrivmsg(int fd, std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        clients[fd].set_response(FORMAT_RESPONSE("461", "PRIVMSG :Not enough parameters"));
        return;
    }

    std::string target = args[1];
    std::string message;
    for (std::vector<std::string>::iterator it = args.begin() + 2; it != args.end(); ++it)
    {
        if (it != args.begin() + 2)
            message += " ";
        message += *it;
    }

    std::string msg = ":" + clients[fd].get_nick() + " PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#')
    {
        // Channel message
        for (std::vector<channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if (it->getName() == target)
            {
                const std::vector<_client>& members = it->getMembers();
                for (std::vector<_client>::const_iterator mit = members.begin(); mit != members.end(); ++mit)
                {
                    if (mit->get_fd() != fd)
                        clients[mit->get_fd()].set_response(msg);
                }
                return;
            }
        }
        clients[fd].set_response(ERR_NOSUCHNICK);
    }
    else
    {
        // Private message
        bool found = false;
        for (std::map<int, _client>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second.get_nick() == target)
            {
                it->second.set_response(msg);
                found = true;
                break;
            }
        }
        if (!found)
            clients[fd].set_response(ERR_NOSUCHNICK);
    }
}
