/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestMacros.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 08:39:20 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 08:39:20 by mlamkadm         ###   ########.fr       */
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
#define CMD_PASS    "PASS"
#define CMD_NICK    "NICK"
#define CMD_USER    "USER"
#define CMD_JOIN    "JOIN"
#define CMD_PART    "PART"
#define CMD_KICK    "KICK"
#define CMD_INVITE  "INVITE"
#define CMD_PRIVMSG "PRIVMSG"
#define CMD_NOTICE  "NOTICE"
#define CMD_WHOIS   "WHOIS"
#define CMD_LIST    "LIST"
#define CMD_QUIT    "QUIT"
#define CMD_PING    "PING"
#define CMD_PONG    "PONG"

// Response Codes
#define RPL_WELCOME     "001"
#define RPL_YOURHOST    "002"
#define RPL_CREATED     "003"
#define RPL_MYINFO      "004"
#define RPL_LISTSTART   "321"
#define RPL_ENDOFNAMES  "366"
#define ERR_NOSUCHNICK  "401"
#define ERR_NOSUCHCHAN  "403"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREG  "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_UNKNOWNCMD  "421"

#endif
