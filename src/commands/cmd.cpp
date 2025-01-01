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

void CoreServer::cmdPart(int fd, std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "PART failed: No channel specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PART :Not enough parameters"));
        return;
    }

    // Determine the channel name, ensure it starts with '#'
    std::string channelName = args[1];
    if (channelName.empty() || channelName[0] != '#') {
        channelName = "#" + channelName;
    }

    // Check if client exists
    if (clients.find(fd) == clients.end()) {
        std::cout << formatServerMessage("ERROR", "PART failed: Client not found") << std::endl;
        return;
    }

    Client& client = clients[fd];
    std::cout << formatServerMessage("DEBUG", client.getNickName() + " attempting to leave " + channelName) << std::endl;

    bool channelFound = false;

    // Iterate through channels to find the matching channel
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == channelName) {
            channelFound = true;
            // Pass the client's nickname to removeMember
            it->removeMember(client.getNickName());
            break;
        }
    }

    // Handle case where channel was not found
    if (!channelFound) {
        std::cout << formatServerMessage("ERROR", "PART failed: Channel not found") << std::endl;
        client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
        return;
    }

    // Construct part message
    std::string partMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost PART " + channelName + "\r\n";
    client.setResponse(partMsg);

    // Success message
    std::cout << formatServerMessage("SUCCESS", client.getNickName() + " left " + channelName) << std::endl;
}

void CoreServer::cmdPass(int fd, std::vector<std::string> &args) {
    std::cout << formatServerMessage("DEBUG", "Processing PASS command") << std::endl;

    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "PASS command failed: Not enough parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PASS :Not enough parameters"));
        return;
    }

    if (args[1] != ServData.Passwd) {
        std::cout << formatServerMessage("ERROR", "PASS command failed: Invalid password") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_PASSWDMISMATCH, ":Password incorrect"));
        return;
    }

    Client& client = clients[fd];
    client.setPassWord(args[1]);
    client.setAuth(true);
    std::cout << formatServerMessage("DEBUG", "Password accepted") << std::endl;
    client.setResponse(":server NOTICE Auth :Password accepted\r\n");
}

void CoreServer::cmdNick(int fd, std::vector<std::string> &args) {
    std::cout << formatServerMessage("DEBUG", "Processing NICK command") << std::endl;

    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "NICK command failed: No nickname provided") << std::endl;
        clients[fd].setResponse(formatResponse("431", ":No nickname given"));
        return;
    }

    const std::string &nickname = args[1];

    // Check for invalid characters
    if (nickname.find_first_of(" ,*?!@") != std::string::npos) {
        std::cout << formatServerMessage("ERROR", "NICK command failed: Invalid characters in nickname") << std::endl;
        clients[fd].setResponse(formatResponse("432", "* " + nickname + " :Erroneous nickname"));
        return;
    }

    // Check for duplicate nicknames
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->first != fd && it->second.getNickName() == nickname) {
            std::cout << formatServerMessage("ERROR", "NICK command failed: Nickname already in use") << std::endl;
            clients[fd].setResponse(formatResponse("433", "* " + nickname + " :Nickname is already in use"));
            return;
        }
    }

    Client& client = clients[fd];
    std::string oldNick = client.getNickName();
    client.setNickName(nickname);

    if (oldNick.empty()) {
        std::cout << formatServerMessage("DEBUG", "Nickname set to: " + nickname) << std::endl;
        if (client.getAuth() && !client.getFullName().empty()) {
            // Send welcome messages using formatResponse
            client.setResponse(formatResponse(RPL_WELCOME, nickname + " :Welcome to the WeUseArch IRC CHAT."));
            client.setResponse(formatResponse(RPL_YOURHOST, nickname + " :Your host is morpheus-server.ddns.net"));
            client.setResponse(formatResponse(RPL_CREATED, nickname + " :This server was created " __DATE__));
            client.setResponse(formatResponse(RPL_MYINFO, nickname + " :IRC server v1.0"));
        }
    } else {
        std::cout << formatServerMessage("DEBUG", "Nickname changed from " + oldNick + " to " + nickname) << std::endl;
        std::string response = ":" + oldNick + "!" + client.getFullName() + "@localhost NICK :" + nickname + "\r\n";
        client.setResponse(response);
    }
}

void CoreServer::cmdUser(int fd, std::vector<std::string> &args) {
    std::cout << formatServerMessage("DEBUG", "Processing USER command") << std::endl;

    if (args.size() < 5) {
        std::cout << formatServerMessage("ERROR", "USER command failed: Not enough parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "USER :Not enough parameters"));
        return;
    }

    Client& client = clients[fd];

    if (!client.getFullName().empty()) {
        std::cout << formatServerMessage("ERROR", "USER command failed: Already registered") << std::endl;
        client.setResponse(formatResponse(ERR_ALREADYREG, ":You may not reregister"));
        return;
    }

    std::string username = args[1];
    client.setFullName(username);

    if (client.getAuth() && !client.getNickName().empty()) {
        std::string nick = client.getNickName();
        std::cout << formatServerMessage("DEBUG", "Registration complete for " + nick) << std::endl;

        // Send welcome messages using formatResponse
        client.setResponse(formatResponse(RPL_WELCOME, nick + " :Welcome to the IRC Network"));
        client.setResponse(formatResponse(RPL_YOURHOST, nick + " :Your host is irc.example.com"));
        client.setResponse(formatResponse(RPL_CREATED, nick + " :This server was created " __DATE__));
        client.setResponse(formatResponse(RPL_MYINFO, nick + " :IRC server v1.0"));
    }
}

void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "JOIN failed: No channel specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
        return;
    }

    std::string channelName = args[1];
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    Client& client = clients[fd];
    std::cout << formatServerMessage("DEBUG", client.getNickName() + " attempting to join " + channelName) << std::endl;

    // Find or create channel
    bool channelExists = false;
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == channelName) {
            channelExists = true;
            it->addMember(client);
            break;
        }
    }

    if (!channelExists) {
        std::cout << formatServerMessage("DEBUG", "Creating new channel: " + channelName) << std::endl;
        Channel newChannel(channelName);
        newChannel.addMember(client);
        channels.push_back(newChannel);
    }

    // Send join message
    std::string joinMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost JOIN " + channelName + "\r\n";
    client.setResponse(joinMsg);

    std::cout << formatServerMessage("SUCCESS", client.getNickName() + " joined " + channelName) << std::endl;
}

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

void    CoreServer::cmdPong(int fd, std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << formatServerMessage("ERROR", "PONG failed: No parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NOORIGIN, "PONG :No origin specified"));
        return;
    }
    Client& client = clients[fd];
    std::string response = ":" + ServData.ServerName + " PONG " + ServData.ServerName + " :" + args[1] + "\r\n";
    client.setResponse(response);
}

void    CoreServer::cmdPing(int fd, std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << formatServerMessage("ERROR", "PING failed: No parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NOORIGIN, "PING :No origin specified"));
        return;
    }
    Client& client = clients[fd];
    std::string response = ":" + ServData.ServerName + " PING " + ServData.ServerName + " :" + args[1] + "\r\n";
    client.setResponse(response);
}

// void CoreServer::cmdQuit(int fd, std::vector<std::string> &args) {
//     Client& client = clients[fd];
//     std::string quitMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost QUIT :";
//     if (args.size() > 1) {
//         for (std::vector<std::string>::iterator it = args.begin() + 1; it != args.end(); ++it) {
//             if (it != args.begin() + 1)
//                 quitMsg += " ";
//             quitMsg += *it;
//         }
//     }
//     quitMsg += "\r\n";
//     client.setResponse(quitMsg);
//     std::cout << formatServerMessage("INFO", client.getNickName() + " has quit") << std::endl;
//     std::vector<Channel>::iterator channelIt;
//     for (channelIt = channels.begin(); channelIt != channels.end(); ++channelIt) {
//         channelIt->removeMember(client.getNickName());
//     }
//     std::vector<struct pollfd>::iterator new_end = std::remove_if(fds.begin(), fds.end(), FdRemovePredicate(fd));
//     fds.erase(new_end, fds.end());
//     close(fd);
//     clients.erase(fd);
// }


