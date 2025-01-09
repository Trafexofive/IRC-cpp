/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 08:42:52 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 08:42:52 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../inc/Server.hpp"





void CoreServer::cmdPrivmsg(int fd, std::vector<std::string> &args) {
    if (args.size() < 3) {
        std::cout << formatServerMessage("ERROR", "PRIVMSG failed: Incomplete message") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters"));
        return;
    }

    std::string target = args[1];
    std::string message;
    for (std::vector<std::string>::iterator it = args.begin() + 2; it != args.end(); ++it) {
        if (it != args.begin() + 2)
            message += " ";
        message += *it;
    }

    Client& client = clients[fd];
    std::cout << formatServerMessage("MESSAGE", client.getNickName() + " -> " + target + ": " + message) << std::endl;

    std::string response = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        // Channel message
        bool channelFound = false;
        for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
            if (it->getName() == target) {
                channelFound = true;
                const std::vector<Client> &members = it->getMembers();
                for (std::vector<Client>::const_iterator member = members.begin(); member != members.end(); ++member) {
                    if (member->getNickName() != client.getNickName()) {
                        std::map<int, Client>::iterator clientIt = clients.find(member->getFd());
                        if (clientIt != clients.end())
                            clientIt->second.setResponse(response);
                    }
                }
                break;
            }
        }

        if (!channelFound)
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, target + " :No such channel"));
    } else {
        // Private message
        bool recipientFound = false;
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.getNickName() == target) {
                recipientFound = true;
                it->second.setResponse(response);
                break;
            }
        }

        if (!recipientFound)
            client.setResponse(formatResponse(ERR_NOSUCHNICK, target + " :No such nick/channel"));
    }
}



static std::string constructQuitMessage(const Client &client, const std::vector<std::string> &args) {
    std::string quitMsg = ":" + client.getNickName() + "!"
                        + client.getFullName() + "@localhost QUIT :";
    if (args.size() > 1) {
        std::vector<std::string>::const_iterator it = args.begin() + 1;
        for (; it != args.end(); ++it) {
            if (it != args.begin() + 1) {
                quitMsg += " ";
            }
            quitMsg += *it;
        }
    }
    quitMsg += "\r\n";
    return quitMsg;
}


static void removeClientFromAllChannels(const std::string &nickName, std::vector<Channel> &channels) {
    for (std::vector<Channel>::iterator channelIt = channels.begin(); channelIt != channels.end(); ++channelIt) {
        channelIt->removeMember(nickName);
    }
}

void CoreServer::cmdQuit(int fd, std::vector<std::string> &args) {
    Client &client = clients[fd];
    std::string quitMsg = constructQuitMessage(client, args);
    client.setResponse(quitMsg);
    std::cout << formatServerMessage("INFO", client.getNickName() + " has quit") << std::endl;
    removeClientFromAllChannels(client.getNickName(), channels);
    std::vector<struct pollfd>::iterator new_end = std::remove_if(fds.begin(), fds.end(), FdPredicate(fd));
    fds.erase(new_end, fds.end());
    close(fd);
    clients.erase(fd);
}


// I need to understand this better, cant properly inplement for now
void CoreServer::cmdCap(int fd, std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "CAP failed: No subcommand specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "CAP :Not enough parameters"));
        return;
    }
    std::string subcommand = args[1];
    if (subcommand == "LS") {
        std::string response = ":server CAP " + clients[fd].getNickName() + " LS :multi-prefix sasl\r\n";
        clients[fd].setResponse(response);
    } else if (subcommand == "REQ") {
        if (args.size() < 3) {
            std::cout << formatServerMessage("ERROR", "CAP REQ failed: No capability specified") << std::endl;
            clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "CAP REQ :Not enough parameters"));
            return;
        }
        std::string capability = args[2];
        if (capability == "sasl") {
            std::string response = ":server CAP " + clients[fd].getNickName() + " ACK :sasl\r\n";
            clients[fd].setResponse(response);
        } else {
            std::cout << formatServerMessage("ERROR", "CAP REQ failed: Invalid capability") << std::endl;
            clients[fd].setResponse(formatResponse(ERR_INVALIDCAP, capability + " :Invalid capability"));
        }
    } else if (subcommand == "END") {
        std::string response = ":server CAP " + clients[fd].getNickName() + " ACK :multi-prefix sasl\r\n";
        clients[fd].setResponse(response);
    } else {
        std::cout << formatServerMessage("ERROR", "CAP failed: Invalid subcommand") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_INVALIDCAPCMD, subcommand + " :Invalid CAP subcommand"));
    }
}

