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
#include <ctime>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define RESET "\033[0m"

// Helper function for timestamps
static std::string getTime() {
    time_t now = time(NULL);
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&now));
    return std::string(buffer);
}

void CoreServer::cmdUser(int fd, std::vector<std::string>& args)
{
    std::cout << YELLOW "[" << getTime() << "] USER command from fd(" << fd << ")" << RESET << std::endl;

    if (args.size() < 5)
    {
        std::cout << RED "[" << getTime() << "] USER failed: Not enough parameters" << RESET << std::endl;
        clients[fd].set_response(FORMAT_RESPONSE("461", "USER :Not enough parameters"));
        return;
    }

    std::string username = args[1];
    std::string realname = args[4];
    
    std::cout << GREEN "[" << getTime() << "] Setting username for fd(" << fd << "): " 
              << username << RESET << std::endl;
    
    clients[fd].set_user(username);

    if (clients[fd].get_bool() && !clients[fd].get_nick().empty())
    {
        std::cout << GREEN "[" << getTime() << "] Registration complete for " 
                  << clients[fd].get_nick() << RESET << std::endl;
        
        clients[fd].set_response(RPL_WELCOME);
        clients[fd].set_response(RPL_YOURHOST);
        clients[fd].set_response(RPL_CREATED);
        clients[fd].set_response(RPL_MYINFO);
    }
}

void CoreServer::handleCommands(int fd, const std::string& _cmd)
{
    std::cout << BLUE "[" << getTime() << "] CLIENT(" << fd << ") >> " << _cmd << RESET << std::endl;

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
    {
        std::cout << GREEN "[" << getTime() << "] Processing command: " << cmd << RESET << std::endl;
        (this->*(it->second))(fd, args);
    }
    else
    {
        std::cout << RED "[" << getTime() << "] Unknown command: " << cmd << RESET << std::endl;
        clients[fd].set_response(ERR_UNKNOWNCOMMAND);
    }
}

void CoreServer::cmdPass(int fd, std::vector<std::string>& args)
{
    std::cout << YELLOW "[" << getTime() << "] AUTH attempt from fd(" << fd << ")" << RESET << std::endl;
    
    if (args.size() < 2)
    {
        std::cout << RED "[" << getTime() << "] AUTH failed: No password provided" << RESET << std::endl;
        clients[fd].set_response(FORMAT_RESPONSE("461", "PASS :Not enough parameters"));
        return;
    }
    
    if (args[1] != ServData.Passwd)
    {
        std::cout << RED "[" << getTime() << "] AUTH failed: Wrong password" << RESET << std::endl;
        clients[fd].set_response(FORMAT_RESPONSE("464", ":Password incorrect"));
        return;
    }
    
    std::cout << GREEN "[" << getTime() << "] AUTH success for fd(" << fd << ")" << RESET << std::endl;
    clients[fd].set_bool(true);
}

void CoreServer::cmdNick(int fd, std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cout << RED "[" << getTime() << "] NICK failed: No nickname provided" << RESET << std::endl;
        clients[fd].set_response(FORMAT_RESPONSE("461", "NICK :Not enough parameters"));
        return;
    }
    
    const std::string& nickname = args[1];
    
    // Check for invalid characters
    if (nickname.find_first_of(" ,*?!@") != std::string::npos)
    {
        std::cout << RED "[" << getTime() << "] NICK failed: Invalid characters in " << nickname << RESET << std::endl;
        clients[fd].set_response(FORMAT_RESPONSE("432", "* " + nickname + " :Erroneous nickname"));
        return;
    }
    
    // Check if nickname is already in use
    for (std::map<int, _client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.get_nick() == nickname)
        {
            std::cout << RED "[" << getTime() << "] NICK failed: " << nickname << " already in use" << RESET << std::endl;
            clients[fd].set_response(ERR_NICKNAMEINUSE);
            return;
        }
    }

    std::string oldNick = clients[fd].get_nick();
    clients[fd].set_nick(nickname);
    
    std::cout << GREEN "[" << getTime() << "] NICK change: " 
              << (oldNick.empty() ? "(new)" : oldNick) << " -> " << nickname 
              << RESET << std::endl;

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

void CoreServer::cmdJoin(int fd, std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cout << RED "[" << getTime() << "] JOIN failed: No channel specified" << RESET << std::endl;
        clients[fd].set_response(FORMAT_RESPONSE("461", "JOIN :Not enough parameters"));
        return;
    }
    
    std::string channelName = args[1];
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    std::cout << YELLOW "[" << getTime() << "] " << clients[fd].get_nick() 
              << " attempting to join " << channelName << RESET << std::endl;

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
        std::cout << GREEN "[" << getTime() << "] Creating new channel: " << channelName << RESET << std::endl;
        channel newChannel(channelName);
        newChannel.addMember(clients[fd]);
        channels.push_back(newChannel);
    }

    std::cout << GREEN "[" << getTime() << "] " << clients[fd].get_nick() 
              << " joined " << channelName << RESET << std::endl;

    // Rest of your JOIN implementation...
}

void CoreServer::cmdPrivmsg(int fd, std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::cout << RED "[" << getTime() << "] PRIVMSG failed: Incomplete message" << RESET << std::endl;
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

    std::cout << MAGENTA "[" << getTime() << "] " << clients[fd].get_nick() 
              << " -> " << target << ": " << message << RESET << std::endl;

    // Rest of your PRIVMSG implementation...
}
