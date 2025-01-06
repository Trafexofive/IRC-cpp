/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 11:48:07 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/01 11:48:07 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "Utils.hpp"
#include <algorithm>
#include <sstream>

#include "../inc/Helpers.hpp"


typedef struct {
    enum TYPE{
        NORMAL,
        OPERATOR,
        UNKNOWN
    };
}
CLIENT;

class Client
{
private:
    int fdClient;
    bool auth;
    bool connected;
    struct sockaddr_in clientInfos;
    std::string ipAddr;
    std::string fullName;
    std::string nickName;
    std::string passWord;
    std::string buff;
    std::string response;
    int clientType;

public:
    Client();
    Client(int fd, struct sockaddr_in ddr);
    
    
    // Getters
    int getFd() const;
    bool getAuth() const;
    bool getConnected() const;
    const std::string& getIpAddr() const;
    const std::string& getFullName() const;
    const std::string& getNickName() const;
    const std::string& getPassWord() const;
    const std::string& getBuff() const;
    const std::string& getResponse() const;
    const struct sockaddr_in& getClientInfos() const;

    // Setters
    void setFd(int fd);
    void setAuth(bool auth);
    void setConnected(bool connected);
    void setIpAddr(const std::string& ip);
    void setFullName(const std::string& user);
    void setNickName(const std::string& nick);
    void setPassWord(const std::string& pass);
    void setResponse(const std::string& response);
    void setClientInfos(const struct sockaddr_in& info);
    void setBuff(const std::string& buff, bool append = false);

    // Class specific methods
    void sendMessage(const std::string& message);
    std::string receiveMessage();
    void disconnect();
    bool isAuthenticated() const;
    void updateNickName(const std::string& newNickName);
    void logActivity(const std::string& activity);
    void resetPassword(const std::string& newPassword);

    void authenticate();
    void clear();
    void clearResponse();
};

#endif
