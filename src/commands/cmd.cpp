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
#include <vector>
#include <stdexcept>
#include <iostream>

// Helper function to check if a name is a valid channel
static bool isChannel(const std::string& name) {
    return (!name.empty() && (name[0] == '#' || name[0] == '&'));
}

// Helper function to get a channel by name
static Channel& getChannel(const std::string& name, std::vector<Channel>& channels) {
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == name)
            return *it;
    }
    throw std::runtime_error("Channel not found");
}

// Helper function to remove a client from a channel
static void removeClientFromChannel(Client& client, Channel& channel) {
    channel.removeMember(client.getNickName());
}

// Helper function to handle invalid channel case
static void handleInvalidChannel(Client& client, const std::string& channelName) {
    std::cout << formatServerMessage("ERROR", "PART failed: Invalid channel name " + channelName) << std::endl;
    client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

// Helper function to handle successful part operation
static void handlePartSuccess(Client& client, const std::string& channelName) {
    std::string partMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost PART " + channelName + "\r\n";
    client.setResponse(partMsg);
    std::cout << formatServerMessage("SUCCESS", client.getNickName() + " left " + channelName) << std::endl;
}

//
// helpers above need to be moved

void CoreServer::cmdPart(int fd, std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << formatServerMessage("ERROR", "PART failed: No channel specified") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "PART :Not enough parameters"));
        return;
    }

    // Check if client exists
    if (clients.find(fd) == clients.end()) {
        std::cout << formatServerMessage("ERROR", "PART failed: Client not found") << std::endl;
        return;
    }

    Client& client = clients[fd];

    // Loop on args to get the channel names
    for (std::vector<std::string>::size_type i = 1; i < args.size(); ++i) {
        std::string channelName = args[i];

        if (!isChannel(channelName)) {
            handleInvalidChannel(client, channelName);
            continue;
        }

        std::cout << formatServerMessage("DEBUG", client.getNickName() + " attempting to leave " + channelName) << std::endl;

        try {
            Channel& channel = getChannel(channelName, channels);
            removeClientFromChannel(client, channel);
            handlePartSuccess(client, channelName);
        } catch (const std::runtime_error& e) {
            std::cout << formatServerMessage("ERROR", "PART failed: " + std::string(e.what())) << std::endl;
            client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :No such channel"));
        }
    }
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
        client.setResponse(formatResponse(RPL_WELCOME, nick + " :Welcome to the WeUseArch IRC Network"));
        client.setResponse(formatResponse(RPL_YOURHOST, nick + " :Your host is morpheus-server.ddns.net"));
        client.setResponse(formatResponse(RPL_CREATED, nick + " :This server was created " __DATE__));
        client.setResponse(formatResponse(RPL_MYINFO, nick + " :IRC server " __VERSION__));
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


void    CoreServer::cmdPing(int fd, std::vector<std::string> &args)
{
    
    long unsigned int i = 0;

    while (i < args.size())
    {
        std::cout << "args[" << i << "] = " << args[i] << std::endl;
        i++;
    }
// Numeric Replies:
//
// -   [`ERR_NEEDMOREPARAMS`](#errneedmoreparams-461) `(461)`
// -   [`ERR_NOORIGIN`](#errnoorigin-409) `(409)`
//
// Deprecated Numeric Reply:
//
// -   [`ERR_NOSUCHSERVER`](#errnosuchserver-402) `(402)`
    
    if (args.size() < 2)
    {
        std::cout << formatServerMessage("ERROR", "PONG failed: No parameters") << std::endl;
        clients[fd].setResponse(formatResponse(ERR_NOORIGIN, "PONG :No origin specified"));
        return;
    }
    Client& client = clients[fd];

    std::string response = "PONG " + args[1] + "\r\n";
    client.setResponse(response);
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

