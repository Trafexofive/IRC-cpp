
#include "../../inc/Server.hpp"

static bool	containsMessage(const std::string &topic)
{
	if (topic[0] == ':' && topic.size() > 1)
		return (true);
	return (false);
}

static std::string &appendArgs(std::vector<std::string> &args,
	std::string &result, int i)
{
	for (; i < args.size(); i++)
	{
		result += args[i];
		if (i + 1 < args.size())
			result += " ";
	}
	printServerMessage("DEBUG", "parsedMessage TOPIC: " + result);
	return (result);
}

void CoreServer::cmdTopic(int fd, std::vector<std::string> &args)
{
	Channel	*channel;

	if (isClientDisconnected(fd))
		return ;
	if (args.size() < 2)
	{
		printServerMessage("DEBUG", "TOPIC failed: Not enough parameters");
		clients[fd].setResponse(formatResponse(ERR_NEEDMOREPARAMS,
				"TOPIC :Not enough parameters"));
		return ;
	}
	Client &client = clients[fd];
	std::string channelName = args[1];
	channel = getChannel(channelName);
	if (channel == NULL)
	{
		printServerMessage("DEBUG", "TOPIC failed: Channel does not exist");
		client.setResponse(formatResponse(ERR_NOSUCHCHANNEL, channelName
				+ " :No such channel"));
		return ;
	} else if (!channel->isMember(client)) {
	    client.setResponse(formatChannelResponse(channelName, client.getTarget(), ERR_NOTONCHANNEL,
	            + "You're not on that channel"));
	    return ;
	}
	if (args.size() == 2)
	{
		if (channel->getTopic().empty())
		{
			client.setResponse(formatChannelResponse(channelName, client.getTarget(), RPL_NOTOPIC, "No topic is set"));
			return ;
		}
		client.setResponse(formatChannelResponse(channelName, client.getTarget(), RPL_TOPIC, "Channel Topic is: " + channel->getTopic()));
		return ;
	}
	else if (args.size() > 2)
	{
		std::string parsedMessage = "";
		appendArgs(args, parsedMessage, 2);
		if (containsMessage(parsedMessage) && !(parsedMessage.size() == 1))
		{
            if (channel->isTopicMode() && !(channel->isOperator(client.getNickName())))
            {
                client.setResponse(formatChannelResponse(channel->getName(), client.getTarget(), ERR_CHANOPRIVSNEEDED,
                "You're not an operator"));
                return ;
            } 
			std::string newTopic = parsedMessage.substr(1);

			channel->setTopic(newTopic);
			std::string topicMsg = formatChannelResponse(channelName, client.getTarget(), RPL_TOPIC, newTopic);
			client.setResponse(topicMsg);
			printServerMessage("DEBUG", "TOPIC: " + client.getNickName()
				+ " changed the topic of " + channelName + " to " + newTopic);
			return ;
		}
		else if (parsedMessage.size() == 1)
		{
			// client.setResponse(formatResponse(RPL_TOPIC, channelName + "Resetting Channel topic for " + channelName));
			client.setResponse(formatChannelResponse(channelName, client.getTarget(), RPL_TOPIC, "TOPIC :Resetting Channel topic for " + channelName));
			channel->setTopic("");
		}
	}
}
