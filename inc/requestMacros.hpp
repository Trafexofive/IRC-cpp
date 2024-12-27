/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestMacros.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 22:36:27 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/07 22:36:27 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTMACROS
#define REQUESTMACROS

// Response formatting macro
#define FORMAT_RESPONSE(code, message) code " " message "\r\n"

// Welcome and Server Information
#define RPL_WELCOME FORMAT_RESPONSE("001", "Welcome to the Internet Relay Chat Network")
#define RPL_YOURHOST FORMAT_RESPONSE("002", "Your host is IRC Server")
#define RPL_CREATED FORMAT_RESPONSE("003", "This server was created on")
#define RPL_MYINFO FORMAT_RESPONSE("004", "MyInfo :0")

// Server Registration
#define RPL_SERVERLIST FORMAT_RESPONSE("251", "Server List :0")
#define RPL_ENDOFWHOIS FORMAT_RESPONSE("318", "End of WHOIS list")
#define RPL_ENDOFNAMES FORMAT_RESPONSE("366", "End of NAMES list")

// Channel-related Responses
#define RPL_LISTSTART FORMAT_RESPONSE("321", "Channel List:")
#define RPL_CHANNELMODEIS FORMAT_RESPONSE("324", "Channel mode is")
#define RPL_BANLIST FORMAT_RESPONSE("367", "Banned users:")
#define RPL_ENDOFBANLIST FORMAT_RESPONSE("368", "End of ban list")
#define RPL_EXCEPTLIST FORMAT_RESPONSE("346", "Except list:")
#define RPL_ENDOFEXCEPTLIST FORMAT_RESPONSE("347", "End of except list")
#define RPL_INVITELIST FORMAT_RESPONSE("346", "Invite list:")
#define RPL_ENDOFINVITELIST FORMAT_RESPONSE("347", "End of invite list")

// Error Responses
#define ERR_NOSUCHNICK FORMAT_RESPONSE("401", "No such nick/channel")
#define ERR_UNKNOWNCOMMAND FORMAT_RESPONSE("421", "Unknown command")
#define ERR_NOMOTD FORMAT_RESPONSE("422", "MOTD File is missing")
#define ERR_NICKNAMEINUSE FORMAT_RESPONSE("433", "Nickname is already in use")

// Custom Responses
#define CUSTOM_RPL_YOURSCORE FORMAT_RESPONSE("800", "You scored")
#define CUSTOM_RPL_CUMETIME FORMAT_RESPONSE("801", "Current time is")
#define CUSTOM_RPL_LOCALTIME FORMAT_RESPONSE("802", "Local time is")
#define CUSTOM_RPL_GLINE FORMAT_RESPONSE("803", "You are now banned")
#define CUSTOM_RPL_UNIQOPATTRS FORMAT_RESPONSE("804", "You are now unique-op")

// Connection/Registration
#define CMD_USER "USER"
#define CMD_NICK "NICK"
#define CMD_PASS "PASS"
#define CMD_OPER "OPER"

// Channel Operations  
#define CMD_JOIN "JOIN"
#define CMD_PART "PART"
#define CMD_KICK "KICK"
#define CMD_INVITE "INVITE"

// Messaging
#define CMD_PRIVMSG "PRIVMSG"
#define CMD_NOTICE "NOTICE"

// Server Queries
#define CMD_WHOIS "WHOIS"
#define CMD_WHOWAS "WHOWAS"
#define CMD_LIST "LIST"

// Miscellaneous
#define CMD_PING "PING"
#define CMD_PONG "PONG"
#define CMD_QUIT "QUIT"

#endif
