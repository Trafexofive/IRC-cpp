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

#include "Helpers.hpp"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>

// Basic formatting helper function (not a macro)
inline std::string formatResponse(const std::string& code, const std::string& message)
{
    return code + " " + message + "\r\n";
}


namespace StringUtils {

static const std::string LINE_ENDING = "\r\n";

struct Severity {
    enum Type {
        INFO,
        WARNING,
        ERROR,
        DEBUG,
        FATAL
    };
};

// Converts a severity enum to its string representation.
inline std::string getSeverityString(Severity::Type severity) {
    switch (severity) {
        case Severity::INFO: return "INFO";
        case Severity::WARNING: return "WARNING";
        case Severity::DEBUG: return "DEBUG";
        case Severity::ERROR: return "ERROR";
        case Severity::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

// Formats a message string with severity, code, and content.
inline std::string formatMessage(Severity::Type severity,
                                 const std::string &code,
                                 const std::string &message) {
    return getSeverityString(severity) + " " + code + ": " + message + LINE_ENDING;
}

// Specialized error message formatting.
inline std::string formatErrorMessage(const std::string &errorCode,
                                      const std::string &errorMessage) {
    return formatMessage(Severity::ERROR, errorCode, errorMessage);
}

// Specialized success/info message formatting.
inline std::string formatSuccessMessage(const std::string &successCode,
                                        const std::string &details) {
    return formatMessage(Severity::INFO, successCode, details);
}

// Formats multiple lines with an optional indent.
inline std::string formatMultilineMessage(const std::string &identifier,
                                          const std::vector<std::string> &lines,
                                          const std::string &indent = "  ") {
    std::ostringstream oss;
    oss << identifier << LINE_ENDING;
    for (size_t i = 0; i < lines.size(); ++i) {
        oss << indent << lines[i] << LINE_ENDING;
    }
    return oss.str();
}

// Formats a command string, e.g., for sending IRC commands.
inline std::string formatCommand(const std::string &command,
                                 const std::vector<std::string> &args) {
    std::ostringstream oss;
    oss << command;
    for (size_t i = 0; i < args.size(); ++i) {
        // Spaces in args usually require special handling or quotes for IRC commands.
        oss << " " << args[i];
    }
    oss << LINE_ENDING;
    return oss.str();
}

// Formats a log message using a time_t-based timestamp.
inline std::string formatLogMessage(std::time_t timestamp,
                                    const std::string &content,
                                    const std::string &timeFormat = "%Y-%m-%d %H:%M:%S") {
    char timeStr[128];
    std::tm *timeinfo = std::localtime(&timestamp);
    std::strftime(timeStr, sizeof(timeStr), timeFormat.c_str(), timeinfo);
    std::ostringstream oss;
    oss << "[" << timeStr << "] " << content << LINE_ENDING;
    return oss.str();
}

}

// Commands

#define PASS "PASS"
#define NICK "NICK"
#define USER "USER"
#define OPER "OPER"
#define MODE "MODE"
#define SERVICE "SERVICE"
#define QUIT "QUIT"
#define SQUIT "SQUIT"
#define JOIN "JOIN"
#define PART "PART"
#define TOPIC "TOPIC"
#define NAMES "NAMES"
#define LIST "LIST"
#define INVITE "INVITE"
#define KICK "KICK"
#define PRIVMSG "PRIVMSG"
#define NOTICE "NOTICE"
#define MOTD "MOTD"
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
#define KILL "KILL"
#define PING "PING"
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
#define CAP "CAP"

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

// CAP response codes
#define ERR_INVALIDCAPCMD "410"
#define ERR_INVALIDCAP "411"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOTREGISTERED "451"
// #define ERR_UNKNOWNCAP "410"

#endif
