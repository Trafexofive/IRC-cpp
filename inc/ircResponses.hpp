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
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>


#define printServerMessage(type, message) std::cout << formatServerMessage(type, message) << std::endl
#define formatResponse(code, message) ":" + SERVER_NAME + code + " " + message + CRLF
#define formatResponseSource(source, code, message) ":" + SERVER_NAME + source + " " + code + " " + message + CRLF
#define formatResponseDest(dest, code, message) ":" + SERVER_NAME + dest + " " + code + " " + message + CRLF
// #define formatResponseCommand(source, dest, code, message) ":" + source + " " + SERVER_NAME + dest + " " + code + " " + message + CRLF

// Commands

// Available commands
#define PASS "PASS"
#define NICK "NICK"
#define USER "USER"
#define MODE "MODE"
#define QUIT "QUIT"
#define JOIN "JOIN"
#define PART "PART"
#define TOPIC "TOPIC"
#define PING "PING"

// Inprogress commands
#define PRIVMSG "PRIVMSG"
#define LIST "LIST"
#define MOTD "MOTD"
#define CAP "CAP"
#define KILL "KILL"

#define OPER "OPER"
#define SERVICE "SERVICE"
#define SQUIT "SQUIT"
#define NAMES "NAMES"
#define INVITE "INVITE"
#define KICK "KICK"
#define NOTICE "NOTICE"
#define LUSERS "LUSERS"
#define VERSION "VERSION"
#define STATS "STATS"
#define LINKS "LINKS"
#define TIME "TIME"
#define CONNECT "CONNECT"
#define TRACE "TRACE"
#define ADMIN "ADMIN"
#define INFO "INFO"
#define SERVLIST "SERVLIST"
#define SQUERY "SQUERY"
#define WHO "WHO"
#define WHOIS "WHOIS"
#define WHOWAS "WHOWAS"
#define PONG "PONG"
#define ERROR "ERROR"
#define AWAY "AWAY"
#define REHASH "REHASH"
#define DIE "DIE"
#define RESTART "RESTART"
#define SUMMON "SUMMON"
#define USERS "USERS"
#define WALLOPS "WALLOPS"
#define USERHOST "USERHOST"
#define ISON "ISON"


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
#define ERR_NOTREGISTERED       "451"
#define ERR_NEEDMOREPARAMS      "461"
#define ERR_NOORIGIN            "409"

// CAP response codes
#define ERR_INVALIDCAPCMD "410"
#define ERR_INVALIDCAP "411"
#define ERR_NEEDMOREPARAMS "461"
// #define ERR_UNKNOWNCAP "410"


    // ERR_NEEDMOREPARAMS (461)
    // ERR_NOSUCHCHANNEL (403)
    // ERR_NOTONCHANNEL (442)
    // ERR_CHANOPRIVSNEEDED (482)
    // RPL_NOTOPIC (331)
    // RPL_TOPIC (332)
    // RPL_TOPICWHOTIME (333)

#define rpltopic(client, channel, topic) RPC_TOPIC + " " + client + " " + channel + " :" + topic + CRLF




#endif
