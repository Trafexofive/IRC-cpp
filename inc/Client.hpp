/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */ /*   Created: 2025/01/01 11:48:07 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/10 20:14:36 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Utils.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

#include "../inc/Helpers.hpp"

typedef struct {
  enum TYPE { NORMAL, OPERATOR, OWNER, UNKNOWN };
  int state;
} CLEARANCE;

typedef struct {

  enum TYPE { ACTIVE, IDLE, DONOTDISTURB, UNKNOWN, OFFLINE };
  int state;

} STATE;

typedef struct {
  enum TYPE { AUTHENTICATED, REJECTED, REGISTERED, UNKNOWN };
  int state;
} AUTH;

class Client {
private:
  int fdClient;

  bool auth;
  bool connected;

  struct sockaddr_in clientInfos;

  std::string ipAddr;

  std::string fullName; // username
  std::string nickName;
  std::string realName;

  std::string passWord;

  std::string response;

  std::string source; // should be renamed to target

  STATE _state;
  AUTH _status;
  CLEARANCE _clearance;

  int clientType;

public:
  // constructors
  Client(int fd, struct sockaddr_in ddr);
  Client(const Client &obj);
  Client(); // should be private
  ~Client();

  // Getters
  int getFd() const;
  bool getAuth() const;
  bool getConnected() const;

  const std::string &getIpAddr() const;
  const std::string &getFullName() const;
  const std::string &getNickName() const;
  const std::string &getRealName() const { return realName; }
  const std::string &getSource() const { return source; }
  const std::string &getPassWord() const;
  const std::string &getResponse() const;
  const struct sockaddr_in &getClientInfos() const;

  // Setters
  void setFd(int fd);
  void setAuth(bool auth);
  void setConnected(bool connected);
  void setIpAddr(const std::string &ip);
  void setFullName(const std::string &user);
  void setNickName(const std::string &nick);
  void setRealName(const std::string &real) { realName = real; }
  void setSource(const std::string &src) { source = src; }
  void setPassWord(const std::string &pass);
  void setResponse(const std::string &response);
  void setClientInfos(const struct sockaddr_in &info);

  // Class specific methods
  void sendMessage(const std::string &message);
  std::string receiveMessage();
  void disconnect();
  bool isAuthenticated() const;
  void updateNickName(const std::string &newNickName);
  void logActivity(const std::string &activity);
  void resetPassword(const std::string &newPassword);

  void authenticate();
  void clear();
  void clearResponse();

  void setClientType(int type) { clientType = type; }
  void setClientType(std::string type) {
    if (type == "OPERATOR")
      _clearance.state = CLEARANCE::OPERATOR;
    else if (type == "NORMAL")
      _clearance.state = CLEARANCE::NORMAL;
    else if (type == "OWNER")
      _clearance.state = CLEARANCE::OWNER;
    else
      _clearance.state = CLEARANCE::UNKNOWN;
  }
  void constructSource() {
    if (connected) {

      if (nickName.empty()) {
        std::cout << formatServerMessage(
                         "WARNING",
                         "NickName is empty, constructing source regardless")
                  << std::endl;
      }
      if (realName.empty() && ipAddr.empty()) {
        source = nickName;
      } else if (ipAddr.empty()) {
        source = nickName + "!" + realName;
      } else {
        source = nickName + "!" + realName + "@" + ipAddr;
      }

      // Output the constructed source for debugging
      std::cout << formatServerMessage("DEBUG", "source: " + source)
                << std::endl;
    }
  }
  void constructIpAddr() { ipAddr = inet_ntoa(clientInfos.sin_addr); }
  void printClientInfo();
};

#endif
