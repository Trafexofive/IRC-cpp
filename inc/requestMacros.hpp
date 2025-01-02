/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestMacros.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 23:27:22 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/29 23:27:22 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTMACROS_HPP
#define REQUESTMACROS_HPP

#include <string>

// Basic formatting helper function (not a macro)
inline std::string formatResponse(const std::string& code, const std::string& message)
{
    return code + " " + message + "\r\n";
}

// Commands
#define CMD_PASS        "PASS"
#define CMD_NICK        "NICK"
#define CMD_USER        "USER"
#define CMD_OPER        "OPER"
#define CMD_MODE        "MODE"
#define CMD_SERVICE     "SERVICE"
#define CMD_QUIT        "QUIT"
#define CMD_SQUIT       "SQUIT"
#define CMD_JOIN        "JOIN"
#define CMD_PART        "PART"
#define CMD_TOPIC       "TOPIC"
#define CMD_NAMES       "NAMES"
#define CMD_LIST        "LIST"
#define CMD_INVITE      "INVITE"
#define CMD_KICK        "KICK"
#define CMD_PRIVMSG     "PRIVMSG"
#define CMD_NOTICE      "NOTICE"
#define CMD_MOTD        "MOTD"
#define CMD_LUSERS      "LUSERS"
#define CMD_VERSION     "VERSION"
#define CMD_STATS       "STATS"
#define CMD_LINKS       "LINKS"
#define CMD_TIME        "TIME"
#define CMD_CONNECT     "CONNECT"
#define CMD_TRACE       "TRACE"
#define CMD_ADMIN       "ADMIN"
#define CMD_INFO        "INFO"
#define CMD_SERVLIST    "SERVLIST"
#define CMD_SQUERY      "SQUERY"
#define CMD_WHO         "WHO"
#define CMD_WHOIS       "WHOIS"
#define CMD_WHOWAS      "WHOWAS"
#define CMD_KILL        "KILL"
#define CMD_PING        "PING"
#define CMD_PONG        "PONG"
#define CMD_ERROR       "ERROR"
#define CMD_AWAY        "AWAY"
#define CMD_REHASH      "REHASH"
#define CMD_DIE         "DIE"
#define CMD_RESTART     "RESTART"
#define CMD_SUMMON      "SUMMON"
#define CMD_USERS       "USERS"
#define CMD_WALLOPS     "WALLOPS"
#define CMD_USERHOST    "USERHOST"
#define CMD_ISON        "ISON"

// Response Codes
#define RPL_WELCOME             "001"
#define RPL_YOURHOST            "002"
#define RPL_CREATED             "003"
#define RPL_MYINFO              "004"
#define RPL_BOUNCE              "005"
#define RPL_USERHOST            "302"
#define RPL_ISON                "303"
#define RPL_AWAY                "301"
#define RPL_UNAWAY              "305"
#define RPL_NOWAWAY             "306"
#define RPL_WHOISUSER           "311"
#define RPL_WHOISSERVER         "312"
#define RPL_WHOISOPERATOR       "313"
#define RPL_WHOISIDLE           "317"
#define RPL_ENDOFWHOIS          "318"
#define RPL_WHOISCHANNELS       "319"
#define RPL_LISTSTART           "321"
#define RPL_LIST                "322"
#define RPL_LISTEND             "323"
#define RPL_CHANNELMODEIS       "324"
#define RPL_NOTOPIC             "331"
#define RPL_TOPIC               "332"
#define RPL_INVITING            "341"
#define RPL_SUMMONING           "342"
#define RPL_VERSION             "351"
#define RPL_WHOREPLY            "352"
#define RPL_ENDOFWHO            "315"
#define RPL_NAMREPLY            "353"
#define RPL_ENDOFNAMES          "366"
#define RPL_LINKS               "364"
#define RPL_ENDOFLINKS          "365"
#define RPL_BANLIST             "367"
#define RPL_ENDOFBANLIST        "368"
#define RPL_INFO                "371"
#define RPL_ENDOFINFO           "374"
#define RPL_MOTDSTART           "375"
#define RPL_MOTD                "372"
#define RPL_ENDOFMOTD           "376"
#define RPL_YOUREOPER           "381"
#define RPL_REHASHING           "382"
#define RPL_TIME                "391"
#define ERR_NOSUCHNICK          "401"
#define ERR_NOSUCHSERVER        "402"
#define ERR_NOSUCHCHANNEL       "403"
#define ERR_CANNOTSENDTOCHAN    "404"
#define ERR_TOOMANYCHANNELS     "405"
#define ERR_WASNOSUCHNICK       "406"
#define ERR_TOOMANYTARGETS      "407"
#define ERR_NOORIGIN            "409"
#define ERR_NORECIPIENT         "411"
#define ERR_NOTEXTTOSEND        "412"
#define ERR_NOTOPLEVEL          "413"
#define ERR_WILDTOPLEVEL        "414"
#define ERR_UNKNOWNCOMMAND      "421"
#define ERR_NOMOTD              "422"
#define ERR_NOADMININFO         "423"
#define ERR_FILEERROR           "424"
#define ERR_NONICKNAMEGIVEN     "431"
#define ERR_ERRONEUSNICKNAME    "432"
#define ERR_NICKNAMEINUSE       "433"
#define ERR_NICKCOLLISION       "436"
#define ERR_USERNOTINCHANNEL    "441"
#define ERR_NOTONCHANNEL        "442"
#define ERR_USERONCHANNEL       "443"

// Newly added missing response codes:
#define ERR_NEEDMOREPARAMS      "461" // Not enough parameters
#define ERR_PASSWDMISMATCH      "464" // Password mismatch
#define ERR_ALREADYREG          "462" 
#define ERR_NOSUCHCHAN          "403"
#define ERR_UNKNOWNMODE         "472"
#define ERR_PASSWDMISMATCH      "464"
#define ERR_NEEDMOREPARAMS      "461"
#define ERR_NOORIGIN            "409"



#endif
