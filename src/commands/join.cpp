#include "../../inc/Server.hpp"

// IRC response codes
#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_BADCHANNELKEY "475"
#define ERR_INVITEONLYCHAN "473"
#define ERR_CHANNELISFULL "471"
#define ERR_USERONCHANNEL "443"
#define RPL_TOPIC "332"

typedef struct {
    std::string channels;
    std::string keys;
} JOIN_ARGS;

static void handleInvalidChannelName(Client &client, const std::string &channelName) {
    printServerMessage("DEBUG", "JOIN failed: Invalid channel name");
    client.setResponse(formatResponse(ERR_NOSUCHCHANNEL, channelName + " :Invalid channel name"));
}

static bool isValidChannelName(const std::string &channelName) {
    return (!channelName.empty() && (channelName[0] == '#'));
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

static JOIN_ARGS parseJoinParams(const std::vector<std::string> &args) {
    JOIN_ARGS params;

    if (args.size() < 2) {
        return params;
    }

    // Validate channel name format
    if (args[1].empty() || args[1][0] != '#') {
        return params;
    }

    params.channels = args[1];
    if (args.size() > 2) {
        params.keys = args[2];
    }

    // Convert commas to spaces
    std::replace(params.channels.begin(), params.channels.end(), ',', ' ');
    std::replace(params.keys.begin(), params.keys.end(), ',', ' ');
    return params;
}

void CoreServer::joinChannel(Client &client, const std::string &channelName) {
    Channel *channelPtr = getChannel(channelName);

    // If the channel doesn't exist, create it and make the client an operator
    if (!channelPtr) {
        createChannel(channelName, &client);
        Channel &channel = this->channels.rbegin()->second;
        channel.addOperator(&client);
        this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
        return;
    }

    Channel &channel = *channelPtr;

    // Check if the client is already a member of the channel
    if (channel.isMember(client)) {
        printServerMessage("WARNING", client.getNickName() + " is already in channel " + channelName);
        client.setResponse(formatResponse(ERR_USERONCHANNEL, client.getNickName() + " " + channelName + " :is already on channel"));
        return;
    }

    // Check if the channel is invite-only and the client is not invited
    if (channel.getInviteMode() && !channel.isInvited(&client)) {
        printServerMessage("WARNING", "JOIN failed: Channel is invite-only");
        client.setResponse(formatResponse(ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)"));
        return;
    }

    // Check if the channel has a key and the provided key is incorrect
    if (channel.getKeyMode()) {
        printServerMessage("WARNING", "JOIN failed: Channel requires a key");
        client.setResponse(formatResponse(ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)"));
        return;
    }

    // Check if the channel has a user limit and it has been reached
    if (channel.getLimitMode() && channel.getMemberCount() >= channel.getMemberLimit()) {
        printServerMessage("WARNING", "JOIN failed: Channel is full");
        client.setResponse(formatResponse(ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)"));
        return;
    }

    // If all checks pass, add the client to the channel
    channel.addMember(&client);
    this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));

    // Send the channel topic to the client if it exists
    if (!channel.getTopic().empty()) {
        client.setResponse(formatResponse(RPL_TOPIC, channel.getName() + " :" + channel.getTopic()));
    }
}

void CoreServer::joinChannel(Client &client, const std::string &channelName, const std::string &key) {
    Channel *channelPtr = getChannel(channelName);

    // If the channel doesn't exist, create it and make the client an operator
    if (!channelPtr) {
        createChannel(channelName, "", key, &client);
        Channel &channel = this->channels.rbegin()->second;
        channel.addOperator(&client);
        this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));
        return;
    }

    Channel &channel = *channelPtr;

    // Check if the client is already a member of the channel
    if (channel.isMember(client)) {
        printServerMessage("WARNING", client.getNickName() + " is already in channel " + channelName);
        client.setResponse(formatResponse(ERR_USERONCHANNEL, client.getNickName() + " " + channelName + " :is already on channel"));
        return;
    }

    // Check if the channel is invite-only and the client is not invited
    if (channel.getInviteMode() && !channel.isInvited(&client)) {
        printServerMessage("WARNING", "JOIN failed: Channel is invite-only");
        client.setResponse(formatResponse(ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)"));
        return;
    }

    // Check if the channel has a key and the provided key is incorrect
    if (channel.getKeyMode() && !channel.validatePassword(key)) {
        printServerMessage("WARNING", "JOIN failed: Invalid channel key");
        client.setResponse(formatResponse(ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)"));
        return;
    }

    // Check if the channel has a user limit and it has been reached
    if (channel.getLimitMode() && channel.getMemberCount() >= channel.getMemberLimit()) {
        printServerMessage("WARNING", "JOIN failed: Channel is full");
        client.setResponse(formatResponse(ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)"));
        return;
    }

    // If all checks pass, add the client to the channel
    channel.addMember(&client);
    this->broadcast(channel, formatBroadcastMessage(client.getTarget(), "JOIN", channel.getName()));

    // Send the channel topic to the client if it exists
    if (!channel.getTopic().empty()) {
        client.setResponse(formatResponse(RPL_TOPIC, channel.getName() + " :" + channel.getTopic()));
    }
}

void CoreServer::cmdJoin(int fd, std::vector<std::string> &args) {
    if (isClientDisconnected(fd)) {
        return;
    }

    Client &client = clients[fd];
    if (args.size() < 2) {
        client.setResponse(formatResponse(ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"));
        return;
    }

    JOIN_ARGS params = parseJoinParams(args);
    std::vector<std::string> channelNames = splitString(params.channels, ' ');
    std::vector<std::string> keys = splitString(params.keys, ' ');

    for (size_t i = 0; i < channelNames.size(); ++i) {
        if (!isValidChannelName(channelNames[i])) {
            handleInvalidChannelName(client, channelNames[i]);
            continue;
        }

        std::string key;
        if (i < keys.size()) {
            key = keys[i];
        }

        if (key.empty()) {
            joinChannel(client, channelNames[i]);
        } else {
            joinChannel(client, channelNames[i], key);
        }
    }
}
