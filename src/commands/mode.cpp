#include "../../inc/Server.hpp"

#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOPRIVILEGES "481"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CHANOPRIVSNEEDED "482"
#define RPL_CHANNELMODEIS "324"
#define ERR_UNKNOWNMODE "472"
#define ERR_NOSUCHNICK "401"

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
            clients[fd].setResponse(
                formatResponse(ERR_NOSUCHCHANNEL, target + " :No such channel"));
            return;
        }

        // If no mode specified, return the current channel mode.
        if (args.size() == 2) {
            clients[fd].setResponse(
                formatResponse(RPL_CHANNELMODEIS, target + " " + channel->getMode()));
            return;
        }
        // Only channel operators can change channel modes.
        if (!channel->isOperator(clients[fd].getNickName())) {
            clients[fd].setResponse(
                formatResponse(ERR_CHANOPRIVSNEEDED, "MODE :You're not an operator"));
            return;
        }

        std::string modeCommand = args[2];
        if (modeCommand.empty()) {
            clients[fd].setResponse(
                formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
            return;
        }

        printServerMessage("INFO", "Client " + clients[fd].getNickName() + " is authorized to use MODE");

        // We'll accumulate the mode letters and their parameters.
        std::string modeSign;               // Either "+" or "-"
        std::string modeLetters;            // E.g. "oitk"
        std::vector<std::string> modeParams;  // Parameters for those modes, if any.

        size_t argIndex = 3; // starting index for mode parameters

        // The mode string should start with a '+' or '-' sign.
        if (modeCommand[0] == '+' || modeCommand[0] == '-') {
            modeSign = modeCommand.substr(0, 1); // save the sign
            // Process each mode letter after the sign.
            for (size_t i = 1; i < modeCommand.size(); i++) {
                char modeChar = modeCommand[i];
                if (modeSign == "+") {
                    switch (modeChar) {
                        case 'o': {
                            if (argIndex >= args.size()) {
                                clients[fd].setResponse(
                                    formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
                                return;
                            }
                            std::string nick = args[argIndex++];
                            Client* targetClient = channel->getClient(nick);
                            if (!targetClient) {
                                clients[fd].setResponse(
                                    formatResponse(ERR_NOSUCHNICK, nick + " :Target Not on Channel"));
                                return;
                            }
                            if (channel->isOperator(targetClient->getNickName())) {
                                clients[fd].setResponse(
                                    formatResponse(ERR_CHANOPRIVSNEEDED, nick + " :is already a channel operator"));
                                return;
                            }
                            channel->addOperator(targetClient);
                            modeLetters.push_back('o');
                            modeParams.push_back(nick);
                            break;
                        }
                        case 'i': {
                            channel->setInviteMode(true);
                            modeLetters.push_back('i');
                            break;
                        }
                        case 't': {
                            channel->setTopicMode(true);
                            modeLetters.push_back('t');
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
                            modeLetters.push_back('k');
                            modeParams.push_back(key);
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
                            modeLetters.push_back('l');
                            modeParams.push_back(limitStr);
                            break;
                        }
                        default: {
                            clients[fd].setResponse(
                                formatResponse(ERR_UNKNOWNMODE, std::string(1, modeChar) + " :is not a recognised channel mode"));
                            return;
                        }
                    }
                }
                else if (modeSign == "-") {
                    switch (modeChar) {
                        case 'o': {
                            if (argIndex >= args.size()) {
                                clients[fd].setResponse(
                                    formatResponse(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters"));
                                return;
                            }
                            std::string nick = args[argIndex++];
                            Client* targetClient = channel->getClient(nick);
                            if (!targetClient) {
                                clients[fd].setResponse(
                                    formatResponse(ERR_NOSUCHNICK, nick + " :No such nick"));
                                return;
                            }
                            if (!channel->isOperator(targetClient->getNickName())) {
                                clients[fd].setResponse(
                                    formatResponse(ERR_CHANOPRIVSNEEDED, nick + " :is not a channel operator"));
                                return;
                            }
                            channel->removeOperator(targetClient);
                            modeLetters.push_back('o');
                            modeParams.push_back(nick);
                            break;
                        }
                        case 'i': {
                            channel->setInviteMode(false);
                            modeLetters.push_back('i');
                            break;
                        }
                        case 't': {
                            channel->setTopicMode(false);
                            modeLetters.push_back('t');
                            break;
                        }
                        case 'k': {
                            channel->setPassword("");
                            channel->setKeyMode(false);
                            channel->setChannelType(CHANNEL::PUBLIC);
                            modeLetters.push_back('k');
                            break;
                        }
                        case 'l': {
                            channel->setMemberLimit(0);
                            channel->setLimitMode(false);
                            modeLetters.push_back('l');
                            break;
                        }
                        default: {
                            clients[fd].setResponse(
                                formatResponse(ERR_UNKNOWNMODE, std::string(1, modeChar) + " :is not a recognised channel mode"));
                            return;
                        }
                    }
                }
            }

            // Compose the final mode string.
            // The format is: "<target> <sign><letters> [params...]"
            std::string finalModeStr = target + " " + modeSign + modeLetters;
            if (!modeParams.empty()) {
                finalModeStr += " ";
                for (size_t i = 0; i < modeParams.size(); i++) {
                    finalModeStr += modeParams[i];
                    if (i < modeParams.size() - 1) {
                        finalModeStr += " ";
                    }
                }
            }
            clients[fd].setResponse(formatBroadcastMessage(clients[fd].getTarget(), "MODE", finalModeStr));
            return;
        }
    } else {
        // Handle user mode or other types if needed.
    }
}
