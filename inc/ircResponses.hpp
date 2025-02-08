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
#define ERR_BADCHANNELKEY "475"

// CAP response codes
#define ERR_INVALIDCAPCMD "410"
#define ERR_INVALIDCAP "411"
#define ERR_NEEDMOREPARAMS "461"
// #define ERR_UNKNOWNCAP "410"

#define formatCommandResponse(source, code, message) ":" + source + " " + SERVER_NAME + " " + code + " " + message + CRLF

#define formatClientResponse(source, code, message) SERVER_NAME + source + " " + code + " " + message + CRLF


#define printServerMessage(type, message) std::cout << formatServerMessage(type, message) << std::endl

#define formatResponse(code, message) SERVER_NAME + code + " " + message + CRLF

#define formatResponseSource(source, code, message) SERVER_NAME + source + " " + code + " " + message + CRLF

#define formatResponseDest(dest, code, message) SERVER_NAME + dest + " " + code + " " + message + CRLF

#define formatCmdResponse(target, command, message) SERVER_NAME + target + " " + command + " " + message + CRLF // solid

// #define formatJoinRespons
// all join responses
// #define ERR_CHANOPRIVSNEEDED 482
// #define ERR_NEEDMOREPARAMS 461
// #define ERR_NOSUCHCHANNEL 403
// #define ERR_NOTONCHANNEL 442
// #define RPL_NOTOPIC 331
// #define RPL_TOPIC 332
// #define RPL_TOPICWHOTIME 333

#define RPLTOPIC(client, channel, topic) SERVER_NAME + RPC_TOPIC + " " + client + " " + channel + " :" + topic + CRLF

#define rplnosuchnick(nick) SERVER_NAME + ERR_NOSUCHNICK + " " + nick + " :No such nick/channel" + CRLF




// common error responses

#define ERR_NOSUCHNICK_MSG(nick) SERVER_NAME + ERR_NOSUCHNICK + " " + nick + " :No such nick/channel" + CRLF
#define ERR_NOSUCHCHANNEL_MSG(client, channel) SERVER_NAME + ERR_NOSUCHCHANNEL + " " + client + " " + channel + " :No such channel" + CRLF
#define ERR_NOTONCHANNEL_MSG(channel) SERVER_NAME + ERR_NOTONCHANNEL + " " + channel + " :You're not on that channel" + CRLF

#define ERR_NEEDMOREPARAMS_MSG(client, command) SERVER_NAME + ERR_NEEDMOREPARAMS + " " + client + " " + command + " :Not enough parameters" + CRLF

#define ERR_TOOMANYARGS_MSG(client, command) SERVER_NAME + ERR_TOOMANYARGS + " " + client + " " + command + " :Too many arguments" + CRLF

#define ERR_PASSWDMISMATCH_MSG(client) SERVER_NAME + ERR_PASSWDMISMATCH + " " + client + " :Password incorrect" + CRLF

#define ERR_ALREADYREG_MSG(client) SERVER_NAME + ERR_ALREADYREG + " " + client + " :You may not reregister" + CRLF

#define ERR_USERNOTINCHANNEL_MSG(client, channel) SERVER_NAME + ERR_USERNOTINCHANNEL + " " + client + " " + channel + " :They aren't on that channel" + CRLF

#define ERR_NOTREGISTERED_MSG(client) SERVER_NAME + ERR_NOTREGISTERED + " " + client + " :You have not registered" + CRLF

#define ERR_NOORIGIN_MSG(client) SERVER_NAME + ERR_NOORIGIN + " " + client + " :No origin specified" + CRLF

#define ERR_USERONCHANNEL_MSG(client, channel) SERVER_NAME + ERR_USERONCHANNEL + " " + client + " " + channel + " :is already on channel" + CRLF

#define ERR_BADCHANNELKEY_MSG(client, channel) SERVER_NAME + ERR_BADCHANNELKEY + " " + client + " " + channel + " :Cannot join channel (+k)" + CRLF

#define RPLTOPICWHOTIME(client, channel, nick, time) SERVER_NAME + RPC_TOPIC + " " + client + " " + channel + " " + nick + " " + time + CRLF
// channel responses

// #define formatBroadcastMessage(client, message) ":" + client + " PRIVMSG " + message + CRLF
#define formatBroadcastMessage(client, command, message) ":" + client + " " + command + " " + message + CRLF

#endif
