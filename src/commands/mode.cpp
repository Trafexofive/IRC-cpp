#include "../../inc/Server.hpp"

#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOPRIVILEGES "481"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CHANOPRIVSNEEDED "482"
#define RPL_CHANNELMODEIS "324"
#define RPL_UMODEIS "221"

// void CoreServer::cmdMode(int fd, std::vector<std::string> &args) {
//   if (isClientDisconnected(fd)) {
//     return;
//   }
//   
//   if (args.size() < 2) {
//     clients[fd].setResponse(
//         formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
//     return;
//   }
//
//   std::string target = args[1];
//
//   // Channel mode handling
//   if (target[0] == '#') {
//     Channel *channel = getChannel(target);
//     if (!channel) {
//       printServerMessage("ERROR", "MODE failed: Channel not found");
//       clients[fd].setResponse(formatResponse(ERR_NOSUCHCHANNEL, target + " :No such channel"));
//       return;
//     }
//
//     // Only channel operators can change channel modes
//     if (!channel->isOperator(&clients[fd])) {
//       clients[fd].setResponse(
//           formatResponse(ERR_CHANOPRIVSNEEDED, "MODE :You're not an operator"));
//       return;
//     }
//     
//     // if no mode specified, return the current channel mode
//     if (args.size() == 2) {
//       clients[fd].setResponse(formatResponse(RPL_CHANNELMODEIS, target + " " + channel->getMode()));
//       return;
//     }
//     
//     std::string mode = args[2];
//     if (mode.empty()) {
//       clients[fd].setResponse(
//           formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
//       return;
//     }
//     
//     // We'll accumulate the responses for each mode change in a single string.
//     std::string finalResponse;
//     
//     // For modes that require an extra parameter (like 'o' or 'k'), we'll use a moving index.
//     size_t argIndex = 3;
//     
//     // Handle add mode ('+')
//     if (mode[0] == '+') {
//       for (size_t i = 1; i < mode.size(); i++) {
//         switch (mode[i]) {
//           case 'o': {
//             if (argIndex >= args.size()) {
//               clients[fd].setResponse(
//                   formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
//               return;
//             }
//             std::string nick = args[argIndex++];
//             // Look up the target client by nickname.
//             Client* targetClient = channel->getClient(nick);
//             if (!targetClient) {
//               clients[fd].setResponse(formatResponse(ERR_NOSUCHNICK, nick + " :No such nick"));
//               return;
//             }
//             if (channel->isOperator(targetClient)) {
//               clients[fd].setResponse(
//                   formatResponse(ERR_CHANOPRIVSNEEDED, nick + " :is already a channel operator"));
//               return;
//             }
//             channel->addOperator(targetClient);
//             finalResponse += formatResponse(RPL_UMODEIS, "+o " + nick) + "\n";
//             break;
//           }
//           case 'i': {
//             channel->setInviteMode(true);
//             finalResponse += formatResponse(RPL_UMODEIS, "+i") + "\n";
//             break;
//           }
//           case 't': {
//             channel->setTopicMode(true);
//             finalResponse += formatResponse(RPL_UMODEIS, "+t") + "\n";
//             break;
//           }
//           case 'k': {
//             if (argIndex >= args.size()) {
//               clients[fd].setResponse(
//                   formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
//               return;
//             }
//             std::string key = args[argIndex++];
//             if (key.empty()) {
//               clients[fd].setResponse(
//                   formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters +k"));
//               return;
//             }
//             channel->setPassword(key);
//             channel->setKeyMode(true);
//             channel->setChannelType(CHANNEL::PRIVATE);
//             printServerMessage("DEBUG", "Channel key set");
//             finalResponse += formatResponse(RPL_UMODEIS, "+k " + key) + "\n";
//             break;
//           }
//           default:
//             // Option not handled: ignore.
//             break;
//         }
//       }
//       clients[fd].setResponse(finalResponse);
//       return;
//     }
//     // Handle remove mode ('-')
//     else if (mode[0] == '-') {
//       for (size_t i = 1; i < mode.size(); i++) {
//         switch (mode[i]) {
//           case 'o': {
//             if (argIndex >= args.size()) {
//               clients[fd].setResponse(
//                   formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
//               return;
//             }
//             std::string nick = args[argIndex++];
//             Client* targetClient = channel->getClient(nick);
//             if (!targetClient) {
//               clients[fd].setResponse(formatResponse(ERR_NOSUCHNICK, nick + " :No such nick"));
//               return;
//             }
//             if (!channel->isOperator(targetClient)) {
//               clients[fd].setResponse(
//                   formatResponse(ERR_CHANOPRIVSNEEDED, nick + " :is not a channel operator"));
//               return;
//             }
//             channel->removeOperator(targetClient);
//             finalResponse += formatResponse(RPL_UMODEIS, "-o " + nick) + "\n";
//             break;
//           }
//           case 'i': {
//             channel->setInviteMode(false);
//             finalResponse += formatResponse(RPL_UMODEIS, "-i") + "\n";
//             break;
//           }
//           case 't': {
//             channel->setTopicMode(false);
//             finalResponse += formatResponse(RPL_UMODEIS, "-t") + "\n";
//             break;
//           }
//           case 'k': {
//             channel->setPassword("");
//             channel->setKeyMode(false);
//             channel->setChannelType(CHANNEL::PUBLIC);
//             printServerMessage("DEBUG", "Channel key removed");
//             finalResponse += formatResponse(RPL_UMODEIS, "-k ") + "\n";
//             break;
//           }
//           default:
//             // Option not handled: ignore.
//             break;
//         }
//       }
//       clients[fd].setResponse(finalResponse);
//       return;
//     }
//   } else {
//     // User mode or other types can be handled here if needed.
//   }
// }

void CoreServer::cmdMode(int fd, std::vector<std::string> &args) {
    if (isClientDisconnected(fd)) {
        return;
    }

    if (args.size() < 2) {
        clients[fd].setResponse(
            formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
        return;
    }

    std::string target = args[1];

    // Channel mode handling
    if (target[0] == '#') {
        Channel *channel = getChannel(target);
        if (!channel) {
            printServerMessage("ERROR", "MODE failed: Channel not found");
            clients[fd].setResponse(formatResponse(ERR_NOSUCHCHANNEL, target + " :No such channel"));
            return;
        }

        // Only channel operators can change channel modes
        if (!channel->isOperator(&clients[fd])) {
            clients[fd].setResponse(
                formatResponse(ERR_CHANOPRIVSNEEDED, "MODE :You're not an operator"));
            return;
        }

        // If no mode specified, return the current channel mode
        if (args.size() == 2) {
            clients[fd].setResponse(formatResponse(RPL_CHANNELMODEIS, target + " " + channel->getMode()));
            return;
        }

        std::string mode = args[2];
        if (mode.empty()) {
            clients[fd].setResponse(
                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
            return;
        }

        // Accumulate responses for each mode change
        std::string finalResponse;

        // For modes that require an extra parameter (like 'o', 'k', 'l'), use a moving index
        size_t argIndex = 3;

        // Handle add mode ('+')
        if (mode[0] == '+') {
            for (size_t i = 1; i < mode.size(); i++) {
                switch (mode[i]) {
                    case 'o': {
                        if (argIndex >= args.size()) {
                            clients[fd].setResponse(
                                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
                            return;
                        }
                        std::string nick = args[argIndex++];
                        Client* targetClient = channel->getClient(nick);
                        if (!targetClient) {
                            clients[fd].setResponse(formatResponse(ERR_NOSUCHNICK, nick + " :No such nick"));
                            return;
                        }
                        if (channel->isOperator(targetClient)) {
                            clients[fd].setResponse(
                                formatResponse(ERR_CHANOPRIVSNEEDED, nick + " :is already a channel operator"));
                            return;
                        }
                        channel->addOperator(targetClient);
                        finalResponse += formatResponse(RPL_UMODEIS, "+o " + nick) + "\n";
                        break;
                    }
                    case 'i': {
                        channel->setInviteMode(true);
                        finalResponse += formatResponse(RPL_UMODEIS, "+i") + "\n";
                        break;
                    }
                    case 't': {
                        channel->setTopicMode(true);
                        finalResponse += formatResponse(RPL_UMODEIS, "+t") + "\n";
                        break;
                    }
                    case 'k': {
                        if (argIndex >= args.size()) {
                            clients[fd].setResponse(
                                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
                            return;
                        }
                        std::string key = args[argIndex++];
                        if (key.empty()) {
                            clients[fd].setResponse(
                                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters +k"));
                            return;
                        }
                        channel->setPassword(key);
                        channel->setKeyMode(true);
                        channel->setChannelType(CHANNEL::PRIVATE);
                        printServerMessage("DEBUG", "Channel key set");
                        finalResponse += formatResponse(RPL_UMODEIS, "+k " + key) + "\n";
                        break;
                    }
                    case 'l': {
                        if (argIndex >= args.size()) {
                            clients[fd].setResponse(
                                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
                            return;
                        }
                        std::string limitStr = args[argIndex++];
                        std::size_t limit = static_cast<std::size_t>(atoi(limitStr.c_str()));
                        if (limit <= 0) {
                            clients[fd].setResponse(
                                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Invalid limit value"));
                            return;
                        }
                        channel->setMemberLimit(limit);
                        channel->setLimitMode(true);
                        finalResponse += formatResponse(RPL_UMODEIS, "+l " + limitStr) + "\n";
                        break;
                    }
                    default: {
                        clients[fd].setResponse(
                            formatResponse(ERR_UNKNOWNMODE, std::string(1, mode[i]) + " :is not a recognised channel mode"));
                        return;
                    }
                }
            }
            clients[fd].setResponse(finalResponse);
            return;
        }
        // Handle remove mode ('-')
        else if (mode[0] == '-') {
            for (size_t i = 1; i < mode.size(); i++) {
                switch (mode[i]) {
                    case 'o': {
                        if (argIndex >= args.size()) {
                            clients[fd].setResponse(
                                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
                            return;
                        }
                        std::string nick = args[argIndex++];
                        Client* targetClient = channel->getClient(nick);
                        if (!targetClient) {
                            clients[fd].setResponse(formatResponse(ERR_NOSUCHNICK, nick + " :No such nick"));
                            return;
                        }
                        if (!channel->isOperator(targetClient)) {
                            clients[fd].setResponse(
                                formatResponse(ERR_CHANOPRIVSNEEDED, nick + " :is not a channel operator"));
                            return;
                        }
                        channel->removeOperator(targetClient);
                        finalResponse += formatResponse(RPL_UMODEIS, "-o " + nick) + "\n";
                        break;
                    }
                    case 'i': {
                        channel->setInviteMode(false);
                        finalResponse += formatResponse(RPL_UMODEIS, "-i") + "\n";
                        break;
                    }
                    case 't': {
                        channel->setTopicMode(false);
                        finalResponse += formatResponse(RPL_UMODEIS, "-t") + "\n";
                        break;
                    }
                    case 'k': {
                        channel->setPassword("");
                        channel->setKeyMode(false);
                        channel->setChannelType(CHANNEL::PUBLIC);
                        printServerMessage("DEBUG", "Channel key removed");
                        finalResponse += formatResponse(RPL_UMODEIS, "-k") + "\n";
                        break;
                    }
                    case 'l': {
                        channel->setMemberLimit(0);
                        channel->setLimitMode(false);
                        finalResponse += formatResponse(RPL_UMODEIS, "-l") + "\n";
                        break;
                    }
                    default: {
                        clients[fd].setResponse(
                            formatResponse(ERR_UNKNOWNMODE, std::string(1, mode[i]) + " :is not a recognised channel mode"));
                        return;
                    }
                }
            }
            clients[fd].setResponse(finalResponse);
            return;
        }
    } else {
        // User mode or other types can be handled here if needed.
    }
}
