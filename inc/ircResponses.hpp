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
#include <sstream>
#include <string>


namespace irc_responses {

// -------------------------------------------------------
// WELCOME RESPONSES
// -------------------------------------------------------
inline std::string getWelcomeMessages(const std::string &nickname) {
    std::ostringstream oss;
    oss << formatResponse(RPL_WELCOME,
                          nickname + " :Welcome to the Internet Relay Chat Network")
        << formatResponse(RPL_YOURHOST, nickname + " :Your host is IRC Server")
        << formatResponse(RPL_CREATED,
                          nickname + " :This server was created " __DATE__)
        << formatResponse(RPL_MYINFO, nickname + " :IRC server v1.0");
    return oss.str();
}

// -------------------------------------------------------
// AUTHENTICATION RESPONSES
// -------------------------------------------------------
inline std::string getPasswordRequired() {
    return formatResponse(ERR_PASSWDMISMATCH, ":Password required");
}

inline std::string getPasswordIncorrect() {
    return formatResponse(ERR_PASSWDMISMATCH, ":Password incorrect");
}

inline std::string getPasswordAccepted() {
    return ":server NOTICE Auth :Password accepted\r\n";
}

// -------------------------------------------------------
// NICKNAME RESPONSES
// -------------------------------------------------------
inline std::string getNickInUse(const std::string &nickname) {
    return formatResponse(ERR_NICKNAMEINUSE,
                          "* " + nickname + " :Nickname is already in use");
}

inline std::string getNickChange(const std::string &oldnick,
                                 const std::string &newnick,
                                 const std::string &username) {
    return ":" + oldnick + "!" + username + "@localhost NICK :" + newnick + "\r\n";
}

// -------------------------------------------------------
// CHANNEL RESPONSES
// -------------------------------------------------------
inline std::string getJoinMessage(const std::string &nickname,
                                  const std::string &username,
                                  const std::string &channel) {
    return ":" + nickname + "!" + username + "@localhost JOIN " + channel + "\r\n";
}

inline std::string getNamesList(const std::string &nickname,
                                const std::string &channel,
                                const std::string &names) {
    return formatResponse(RPL_NAMREPLY,
                          nickname + " = " + channel + " :" + names);
}

inline std::string getEndOfNames(const std::string &nickname,
                                 const std::string &channel) {
    return formatResponse(RPL_ENDOFNAMES,
                          nickname + " " + channel + " :End of /NAMES list");
}

// -------------------------------------------------------
// ERROR RESPONSES
// -------------------------------------------------------
inline std::string getUnknownCommand(const std::string &command) {
    return formatResponse(ERR_UNKNOWNCOMMAND, command + " :Unknown command");
}

inline std::string getNeedMoreParams(const std::string &command) {
    return formatResponse(ERR_NEEDMOREPARAMS,
                          command + " :Not enough parameters");
}

// -------------------------------------------------------
// MESSAGE RESPONSES
// -------------------------------------------------------
inline std::string getPrivateMessage(const std::string &sender,
                                     const std::string &target,
                                     const std::string &message) {
    return ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n";
}

// -------------------------------------------------------
// EXTENDED/ADDITIONAL RESPONSES (10x more extensive)
// -------------------------------------------------------

// 1) MOTD (Message of the Day)
inline std::string getMotdStart(const std::string &nickname) {
    return formatResponse(RPL_MOTDSTART,
                          nickname + " :- IRC Server Message of the Day - ");
}

inline std::string getMotd(const std::string &motdLine) {
    return formatResponse(RPL_MOTD, ":- " + motdLine);
}

inline std::string getEndOfMotd(const std::string &nickname) {
    return formatResponse(RPL_ENDOFMOTD,
                          nickname + " :End of MOTD command");
}

// 2) TOPIC RESPONSES
inline std::string getTopic(const std::string &nickname,
                            const std::string &channel,
                            const std::string &topic) {
    return formatResponse(RPL_TOPIC,
                          nickname + " " + channel + " :" + topic);
}

inline std::string getNoTopic(const std::string &nickname,
                              const std::string &channel) {
    return formatResponse(RPL_NOTOPIC,
                          nickname + " " + channel + " :No topic is set");
}

// 3) KICK RESPONSES
inline std::string getKickMessage(const std::string &kicker,
                                  const std::string &channel,
                                  const std::string &kicked,
                                  const std::string &reason) {
    return ":" + kicker + " KICK " + channel + " " + kicked + " :" + reason + "\r\n";
}

// inline std::string getCannotKickOp(const std::string &channel,
//                                    const std::string &nickname) {
//     return formatResponse(ERR_CHANOPRIVSNEEDED,
//                           channel + " :Cannot kick channel operator " + nickname);
// }

// 4) INVITE RESPONSES
inline std::string getInviteNotification(const std::string &inviter,
                                         const std::string &invitee,
                                         const std::string &channel) {
    return ":" + inviter + " INVITE " + invitee + " :" + channel + "\r\n";
}

// inline std::string getInviteOnlyChan(const std::string &channel) {
//     return formatResponse(ERR_INVITEONLYCHAN,
//                           channel + " :Cannot join channel (+i)");
// }

// 5) NOTICE RESPONSES
inline std::string getServerNotice(const std::string &message) {
    // Typically server notices omit user details in prefix
    return ":server NOTICE * :" + message + "\r\n";
}

inline std::string getUserNotice(const std::string &sender,
                                 const std::string &target,
                                 const std::string &message) {
    return ":" + sender + " NOTICE " + target + " :" + message + "\r\n";
}

// 6) PING / PONG RESPONSES
inline std::string getPing(const std::string &server) {
    return "PING :" + server + "\r\n";
}

inline std::string getPong(const std::string &server) {
    return "PONG :" + server + "\r\n";
}

// 7) QUIT RESPONSES
inline std::string getQuitMessage(const std::string &nickname,
                                  const std::string &quitMessage) {
    return ":" + nickname + " QUIT :" + quitMessage + "\r\n";
}

inline std::string getErrorClosingLink(const std::string &nickname,
                                       const std::string &reason) {
    return "ERROR :Closing Link: " + nickname + "[localhost] (" + reason + ")\r\n";
}

// 8) WHOIS RESPONSES
inline std::string getWhoisUser(const std::string &requester,
                                const std::string &nickname,
                                const std::string &username,
                                const std::string &host,
                                const std::string &realName) {
    return formatResponse(RPL_WHOISUSER,
                          requester + " " + nickname + " " + username + " " +
                          host + " * :" + realName);
}

inline std::string getWhoisServer(const std::string &requester,
                                  const std::string &nickname,
                                  const std::string &server,
                                  const std::string &info) {
    return formatResponse(RPL_WHOISSERVER,
                          requester + " " + nickname + " " + server +
                          " :" + info);
}

inline std::string getWhoisEnd(const std::string &requester,
                               const std::string &nickname) {
    return formatResponse(RPL_ENDOFWHOIS,
                          requester + " " + nickname + " :End of WHOIS list");
}

// 9) PRIVMSG ERRORS
inline std::string getCannotSendToChan(const std::string &channel) {
    return formatResponse(ERR_CANNOTSENDTOCHAN,
                          channel + " :Cannot send to channel");
}

inline std::string getNoSuchNick(const std::string &nickname) {
    return formatResponse(ERR_NOSUCHNICK,
                          nickname + " :No such nick/channel");
}

// 10) USER OPER RESPONSES
inline std::string getOperLogin(const std::string &nickname) {
    return formatResponse(RPL_YOUREOPER,
                          nickname + " :You are now an IRC operator");
}

// inline std::string getOperFail(const std::string &nickname) {
//     return formatResponse(ERR_NOOPERHOST,
//                           nickname + " :No O-lines for your host");
// }

} // namespace irc_responses


#endif
