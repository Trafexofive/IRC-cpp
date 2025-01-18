/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 17:29:11 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/06 17:29:11 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

typedef struct {
    std::string channels;
    std::string keys;
} JOIN_PARAMS;

// Helper function to check if a channel name is valid
static bool isValidChannelName(const std::string &channelName) {
    return !channelName.empty() && (channelName[0] == '#' || channelName[0] == '&');
}

// Helper function to split a string into a vector of strings
static std::vector<std::string> splitString(const std::string &input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Helper function to parse JOIN command parameters
static JOIN_PARAMS parseJoinParams(const std::vector<std::string> &args) {
    JOIN_PARAMS params;
    for (size_t i = 1; i < args.size(); ++i) {
        if (isValidChannelName(args[i])) {
            params.channels += args[i] + " ";
        } else {
            params.keys += args[i] + " ";
        }
    }
    return params;
}

// Helper function to replace commas with spaces in JOIN parameters
static void morphParams(JOIN_PARAMS &params) {
    for (size_t i = 0; i < params.channels.length(); ++i) {
        if (params.channels[i] == ',') {
            params.channels[i] = ' ';
        }
    }
    for (size_t i = 0; i < params.keys.length(); ++i) {
        if (params.keys[i] == ',') {
            params.keys[i] = ' ';
        }
    }
}

// Helper function to handle invalid channel names
static void handleInvalidChannelName(Client &client, const std::string &channelName) {
    std::cout << formatServerMessage("ERROR", "JOIN failed: Invalid channel name " + channelName) << std::endl;
    client.setResponse(formatResponse(ERR_NOSUCHCHAN, channelName + " :Invalid channel name"));
}

// Helper function to join a single channel
void CoreServer::joinSingleChannel(Client &client, const std::string &channelName, const std::string &key) {
    if (!isValidChannelName(channelName)) {
        handleInvalidChannelName(client, channelName);
        return;
    }

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

    // Send join message to the client
    std::string joinMsg = ":" + client.getNickName() + "!" + client.getFullName() + "@localhost JOIN " + channelName + "\r\n";
    client.setResponse(joinMsg);
    std::cout << formatServerMessage("INFO", client.getNickName() + " joined " + channelName) << std::endl;
}

// Main JOIN command handler
void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
    Client &client = clients[fd];
    if (!client.isAuthenticated()) {
        std::cout << formatServerMessage("WARNING", "JOIN failed: Client not authenticated") << std::endl;
        client.setResponse(formatResponse(ERR_NOTREGISTERED, "JOIN :You have not registered"));
        return;
    }

    if (args.size() < 2) {
        std::cout << formatServerMessage("WARNING", "JOIN failed: No channel specified") << std::endl;
        client.setResponse(formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
        return;
    }

    JOIN_PARAMS params = parseJoinParams(args);
    morphParams(params);
    client.constructSource();

    std::vector<std::string> channelNames = splitString(params.channels, ' ');
    std::vector<std::string> keys = splitString(params.keys, ' ');

    for (size_t i = 0; i < channelNames.size(); ++i) {
        const std::string &channelName = channelNames[i];
        const std::string &key = (i < keys.size()) ? keys[i] : "";
        joinSingleChannel(client, channelName, key);
    }
}
// =====================================================================================================================

// void CoreServer::cmdList(int fd, std::vector<std::string> &args) {
//     if (args.size() > 1) {
//         std::cout << formatServerMessage("ERROR", "LIST failed: Too many
//         arguments") << std::endl;
//         clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "LIST :Too
//         many arguments")); return;
//     }
//     std::string listMsg = ":server 321 " + clients[fd].getNickName() + "
//     Channel :Users Name\r\n"; for (std::vector<Channel>::iterator it =
//     channels.begin(); it != channels.end(); ++it) {
//         listMsg += ":server 322 " + clients[fd].getNickName() + " " +
//         it->getName() + " " + std::to_string(it->getMembers().size()) + " :No
//         topic\r\n";
//     }
//     listMsg += ":server 323 " + clients[fd].getNickName() + " :End of
//     /LIST\r\n"; clients[fd].setResponse(listMsg); std::cout <<
//     formatServerMessage("SUCCESS", "Sending channel list to " +
//     clients[fd].getNickName()) << std::endl;
// }
// =====================================================================================================================
//

// static void cmdTopic(int fd, std::vector<std::string> &args) {
//   if (args.size() < 2) {
//     std::cout << formatServerMessage("ERROR",
//                                      "TOPIC failed: No channel specified")
//               << std::endl;
//     clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS, "TOPIC :Not
//     enough parameters")); return;
//   }
// }
