/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircResponses.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 08:38:55 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 08:38:55 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_RESPONSES_HPP
#define IRC_RESPONSES_HPP

#include "./requestMacros.hpp"
#include <string>
#include <sstream>

namespace irc_responses
{
    // Welcome Responses
    inline std::string getWelcomeMessages(const std::string& nickname)
    {
        std::ostringstream oss;
        oss << formatResponse(RPL_WELCOME, nickname + " :Welcome to the Internet Relay Chat Network")
            << formatResponse(RPL_YOURHOST, nickname + " :Your host is IRC Server")
            << formatResponse(RPL_CREATED, nickname + " :This server was created " __DATE__)
            << formatResponse(RPL_MYINFO, nickname + " :IRC server v1.0");
        return oss.str();
    }

    // Authentication Responses
    inline std::string getPasswordRequired()
    {
        return formatResponse(ERR_PASSWDMISMATCH, ":Password required");
    }

    inline std::string getPasswordIncorrect()
    {
        return formatResponse(ERR_PASSWDMISMATCH, ":Password incorrect");
    }

    inline std::string getPasswordAccepted()
    {
        return ":server NOTICE Auth :Password accepted\r\n";
    }

    // Nickname Responses
    inline std::string getNickInUse(const std::string& nickname)
    {
        return formatResponse("433", "* " + nickname + " :Nickname is already in use");
    }

    inline std::string getNickChange(const std::string& oldnick, 
                                   const std::string& newnick, 
                                   const std::string& username)
    {
        return ":" + oldnick + "!" + username + "@localhost NICK :" + newnick + "\r\n";
    }

    // Channel Responses
    inline std::string getJoinMessage(const std::string& nickname, 
                                    const std::string& username,
                                    const std::string& channel)
    {
        return ":" + nickname + "!" + username + "@localhost " + CMD_JOIN + " " + channel + "\r\n";
    }

    inline std::string getNamesList(const std::string& nickname,
                                  const std::string& channel,
                                  const std::string& names)
    {
        return formatResponse("353", nickname + " = " + channel + " :" + names);
    }

    inline std::string getEndOfNames(const std::string& nickname,
                                   const std::string& channel)
    {
        return formatResponse("366", nickname + " " + channel + " :End of /NAMES list");
    }

    // Error Responses
    inline std::string getUnknownCommand(const std::string& command)
    {
        return formatResponse(ERR_UNKNOWNCMD, command + " :Unknown command");
    }

    inline std::string getNeedMoreParams(const std::string& command)
    {
        return formatResponse(ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
    }

    // Message Responses
    inline std::string getPrivateMessage(const std::string& sender,
                                       const std::string& target,
                                       const std::string& message)
    {
        return ":" + sender + " " + CMD_PRIVMSG + " " + target + " :" + message + "\r\n";
    }
}

#endif
