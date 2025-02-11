/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 05:00:23 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/30 05:00:26 by mlamkadm         ###   ########.fr       */
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

struct CLEARANCE {
  enum TYPE { NORMAL, OPERATOR, OWNER, UNKNOWN };
  TYPE state;
};

struct STATE {
  enum TYPE { ACTIVE, IDLE, DONOTDISTURB, UNKNOWN, OFFLINE };
  TYPE state;
};

struct STATUS {
  enum TYPE { AUTHENTICATED, DISCONNECTED, REGISTERED, UNKNOWN };
  TYPE state;
};

// simple load balancer
class Client {
private:
  int fdClient;

  struct sockaddr_in clientInfos;
  std::string ipAddr;

  std::string fullName; // username
  std::string nickName;
  std::string realName;

  std::string passWord;
  std::string response;
  bool _operator;

  std::string _target; // should be renamed to target

  STATE _state;
  STATUS _status;
  CLEARANCE _clearance;

public:
  // constructors
  Client(int fd, struct sockaddr_in ddr);
  Client(const Client &obj);
  Client(); // should be private
  ~Client();

  // Getters
  int getFd() const;

  const std::string &getIpAddr() const;
  const std::string &getFullName() const;
  const std::string &getNickName() const;
  const std::string &getRealName() const { return realName; }
  const std::string &getTarget() const { return _target; }
  const std::string &getPassWord() const;
  const std::string &getResponse() const;
  const struct sockaddr_in &getClientInfos() const;
  bool getOperator() { return _operator; }
  void setOperator(bool op) { _operator = op; }

  // Setters
  void setFd(int fd);
  void setIpAddr(const std::string &ip);
  void setFullName(const std::string &user);
  void setNickName(const std::string &nick);
  void setRealName(const std::string &real) { realName = real; }
  void setTarget(const std::string &target) { _target = target; }
  void setPassWord(const std::string &pass);
  void setResponse(const std::string &response);
  void setClientInfos(const struct sockaddr_in &info);
  void  beoper() { _operator = true; }

  // Class specific methods
  void disconnect();

  void updateNickName(const std::string &newNickName);
  void logActivity(const std::string &activity);
  void resetPassword(const std::string &newPassword);

  void setRegistered() { _status.state = STATUS::REGISTERED; }
  void setAuthenticated() { _status.state = STATUS::AUTHENTICATED; }
  void setDisconnected() { _status.state = STATUS::DISCONNECTED; }

  void setState(STATE::TYPE state) { _state.state = state; }
  void setStatus(STATUS::TYPE state) { _status.state = state; }
  void setClearance(CLEARANCE::TYPE state) { _clearance.state = state; }

  bool isState(STATE::TYPE state) const { return _state.state == state; }
  bool isStatus(STATUS::TYPE state) const { return _status.state == state; }
  bool isClearance(CLEARANCE::TYPE state) const {
    return _clearance.state == state;
  }

  // Smp State Getters
  bool isRegistered() const;
  bool isAuthenticated() const;
  bool isDisconnected() const { return _status.state == STATUS::DISCONNECTED; }

  void authenticate();
  void clear();
  void clearResponse();

  void setClientInfos(int fd, struct sockaddr_in ddr) {
    fdClient = fd;
    clientInfos = ddr;
  }

  STATE getState() const { return _state; }
  STATUS getStatus() const { return _status; }
  CLEARANCE getClearance() const { return _clearance; }

  void constructSource() { // should be renamed to constructTarget
                           //

    if (nickName.empty()) {
      std::cout << formatServerMessage(
                       "WARNING",
                       "NickName is empty, constructing source regardless")
                << std::endl;
    }
    if (realName.empty() && ipAddr.empty()) {
      _target = nickName;
    } else if (ipAddr.empty()) {
      _target = nickName + "!" + realName;
    } else {
      _target = nickName + "!" + realName + "@" + ipAddr;
    }

    // Output the constructed source for debugging
    std::cout << formatServerMessage("DEBUG", "source: " + _target)
              << std::endl;
  }

  void constructIpAddr() { ipAddr = inet_ntoa(clientInfos.sin_addr); }

  void printClientInfo() {
    std::ostringstream oss;
    oss << "Client Info: " << "| fd: " << fdClient << " | ip: " << ipAddr
        << " | nick: " << nickName << " | real: " << realName << " | target: "
        << _target << " | status: " << _status.state << " | state: "
        << _state.state << " | clearance: " << _clearance.state;


  }
};

#endif
