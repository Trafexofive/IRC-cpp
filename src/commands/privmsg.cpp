/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:50:09 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/10 19:50:09 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 19:50:09 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/10 19:50:09 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

std::vector<std::string> get_targets(const std::string &target) {
    std::vector<std::string> targets;
    std::stringstream ss(target);
    std::string item;

    while (std::getline(ss, item, ',')) {
        targets.push_back(item);
    }

    return targets;
}

static std::string constructPrivmsg(const std::string &source,
                                        const std::string &dest,std::string message) {
std::string privMsg = ":" + source  + " PRIVMSG " + dest + " " + message + CRLF;
  return privMsg;
}


bool is_channel(const std::string &target) {
    return !target.empty() && (target[0] == '#' || target[0] == '&');
}

void CoreServer::send_message_to_channel(int fd,const std::string &channel, const std::string &message) {
    Channel *chan = getChannel(channel);
    if (chan == NULL || chan->getChannelType() == CHANNEL::EMPTY) {
      std::string msg = formatResponse(ERR_NOSUCHCHANNEL, "No such channel");
      std::cout << formatServerMessage("ERROR", "Channel not found") << std::endl;
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    if (!chan->isMember(clients[fd])) {
    std::string msg = formatResponse(ERR_NOTONCHANNEL, "You're not on that channel");
      std::cout << formatServerMessage("ERROR", "Client not in channel") << std::endl;
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    std::string msg = constructPrivmsg(clients[fd].getTarget(), channel, message);
    std::map<int, ClientEntry>::const_iterator it = chan->getRegistry().begin();
    for (; it != chan->getRegistry().end(); ++it) {
      if (it->first != fd) { // Don't send the message back to the sender
      send(it->first, msg.c_str(), msg.size(), 0);
      }
    }
}

void  CoreServer::send_message_to_user(int fd, const std::string &target, const std::string &message) {

    bool found = false;
    std::map<int, Client>::iterator it = clients.begin();
    for (; it != clients.end(); ++it) {
        if (it->second.getNickName() == target) {
            if (it->second.isStatus(STATUS::DISCONNECTED))
                break ;
            std::string msg = constructPrivmsg(clients[fd].getTarget(), target, message);
            send(it->first, msg.c_str(), msg.size(), 0);
            found = true;
            break;
        }
    }
    if (!found) {
        std::string msg = formatResponse(ERR_NOSUCHNICK, "No such nick/channel");
        std::cout << formatServerMessage("ERROR", "User not found") << std::endl;
        send(fd, msg.c_str(), msg.size(), 0);
    }
}

bool    validmsg(std::vector<std::string> target, std::string message)
{
    if (target.empty() || message.empty())
        return false;
    if (message.at(0) != ':')
        return false;
    std::vector<std::string>::iterator it = target.begin();
    for (; it != target.end(); ++it) {
        if (it->empty() || it->at(0) == ' '|| it->at(0) == ',' || it->at(it->size() - 1) == ',' || it->at(it->size() - 1) == ' ')
            return false;
    }
    return true;
}

 void CoreServer::cmdPrivmsg(int fd, std::vector<std::string> &args)
 {
    if (args.size() < 3 ) {
        std::string msg = formatResponse(ERR_NEEDMOREPARAMS, "Not enough parameters");
        send(fd, msg.c_str(), msg.size(), 0);
        return;
    }
    std::vector<std::string> targets = get_targets(args[1]);
    std::string message;
    for (size_t i = 2; i < args.size(); i++) {
        message += args[i];
        if (i + 1 < args.size()) {
            message += " ";
        }
    }
    if (!validmsg(targets, message))
    {
        std::string msg = formatResponse(ERR_NEEDMOREPARAMS, "Not enough parameters");
        send(fd, msg.c_str(), msg.size(), 0);
        return; 
    }
    std::vector<std::string>::iterator it = targets.begin();
    for (; it != targets.end(); ++it) {
        if (is_channel(*it)) {
            send_message_to_channel(fd, *it, message);
        } else {
            send_message_to_user(fd, *it, message);
        }
    }
}
