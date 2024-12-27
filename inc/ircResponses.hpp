/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_responses.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 22:36:27 by mlamkadm          #+#    #+#           */
/*   Updated: 2024/12/07 22:36:27 by mlamkadm         ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_RESPONSES_HPP
#define IRC_RESPONSES_HPP

#include "requestMacros.hpp"
#include <string>

namespace irc_responses
{
    // Welcome and Registration Responses
    inline std::string getWelcomeResponse(const std::string& nickname)
    {
        return FORMAT_RESPONSE("001", nickname + " :Welcome to the Internet Relay Chat Network") +
               FORMAT_RESPONSE("002", nickname + " :Your host is IRC Server") +
               FORMAT_RESPONSE("003", nickname + " :This server was created today") +
               FORMAT_RESPONSE("004", nickname + " :IRC server v1.0");
    }

    // Error Response Formatters
    inline std::string getErrorResponse(const std::string& nickname, const std::string& error)
    {
        return FORMAT_RESPONSE(error, nickname + " :" + error);
    }

    inline std::string getNickInUseError(const std::string& nickname)
    {
        return FORMAT_RESPONSE("433", "* " + nickname + " :Nickname is already in use");
    }

    inline std::string getUnknownCommandError(const std::string& command)
    {
        return FORMAT_RESPONSE("421", command + " :Unknown command");
    }

    inline std::string getNoSuchNickError(const std::string& nickname)
    {
        return FORMAT_RESPONSE("401", nickname + " :No such nick/channel");
    }

    // Channel Response Formatters
    inline std::string getChannelJoinMessage(const std::string& nickname, 
                                           const std::string& username,
                                           const std::string& hostname, 
                                           const std::string& channel)
    {
        return ":" + nickname + "!" + username + "@" + hostname + " " + CMD_JOIN + " " + channel + "\r\n";
    }

    inline std::string getChannelModeResponse(const std::string& channel, const std::string& modes)
    {
        return FORMAT_RESPONSE("324", channel + " " + modes);
    }

    inline std::string getEndOfNamesResponse(const std::string& nickname, const std::string& channel)
    {
        return FORMAT_RESPONSE("366", nickname + " " + channel + " :End of /NAMES list");
    }

    // Message Formatters
    inline std::string formatPrivateMessage(const std::string& sender,
                                          const std::string& target,
                                          const std::string& message)
    {
        return ":" + sender + " " + CMD_PRIVMSG + " " + target + " :" + message + "\r\n";
    }

    inline std::string formatNoticeMessage(const std::string& sender,
                                         const std::string& target,
                                         const std::string& message)
    {
        return ":" + sender + " " + CMD_NOTICE + " " + target + " :" + message + "\r\n";
    }

    // Channel Operation Responses
    inline std::string getKickMessage(const std::string& kicker,
                                    const std::string& channel,
                                    const std::string& target,
                                    const std::string& reason)
    {
        return ":" + kicker + " " + CMD_KICK + " " + channel + " " + target + " :" + reason + "\r\n";
    }

    inline std::string getPartMessage(const std::string& nickname,
                                    const std::string& channel,
                                    const std::string& reason = "")
    {
        std::string msg = ":" + nickname + " " + CMD_PART + " " + channel;
        if (!reason.empty())
            msg += " :" + reason;
        return msg + "\r\n";
    }

    // Server Query Responses
    inline std::string getWhoisResponse(const std::string& nickname)
    {
        return FORMAT_RESPONSE("311", nickname + " :WHOIS information");
    }

    inline std::string getEndOfWhoisResponse(const std::string& nickname)
    {
        return RPL_ENDOFWHOIS;
    }

    // Custom Response Formatters
    inline std::string getScoreResponse(const std::string& nickname, const std::string& score)
    {
        return FORMAT_RESPONSE("800", nickname + " :Your score is " + score);
    }

    inline std::string getTimeResponse(const std::string& nickname, const std::string& time)
    {
        return FORMAT_RESPONSE("801", nickname + " :Current time is " + time);
    }

    // Authentication Responses
    inline std::string getPasswordRequest()
    {
        return FORMAT_RESPONSE("464", ":Password required");
    }

    inline std::string getAuthenticationError()
    {
        return FORMAT_RESPONSE("464", ":Password incorrect");
    }

    // List Responses
    inline std::string getListStartResponse()
    {
        return RPL_LISTSTART;
    }

    inline std::string getChannelListEntry(const std::string& channel, 
                                         const std::string& userCount,
                                         const std::string& topic)
    {
        return FORMAT_RESPONSE("322", channel + " " + userCount + " :" + topic);
    }

    inline std::string getListEndResponse()
    {
        return FORMAT_RESPONSE("323", ":End of /LIST");
    }

    // Ban List Responses
    inline std::string getBanListEntry(const std::string& channel, 
                                     const std::string& banmask,
                                     const std::string& banner,
                                     const std::string& time)
    {
        return FORMAT_RESPONSE("367", channel + " " + banmask + " " + banner + " " + time);
    }

    inline std::string getEndOfBanList(const std::string& channel)
    {
        return RPL_ENDOFBANLIST;
    }
}

#endif // IRC_RESPONSES_HPP
