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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "Utils.hpp"
#include <algorithm>
#include <sstream>

#include "../inc/Helpers.hpp"

class _client
{
    private:
        int fd_client;
        bool auth;
        bool connected;
        struct sockaddr_in client_infos;
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        std::string buff;
        std::string response;

    public:
        _client();
        _client(int fd, struct sockaddr_in ddr);
        
        void authenticate();
        void clear();
        void clear_response();
        
        // Getters
        int get_fd() const;
        bool get_bool() const;
        bool get_connect() const;
        const std::string& get_ip() const;
        const std::string& get_user() const;
        const std::string& get_nick() const;
        const std::string& get_pass() const;
        const std::string& get_buff() const;
        const std::string& get_response() const;
        const struct sockaddr_in& get_info() const;

        // Setters
        void set_fd(int fd);
        void set_bool(bool i);
        void set_connect(bool i);
        void set_ip(const std::string& ip);
        void set_user(const std::string& user);
        void set_nick(const std::string& nick);
        void set_pass(const std::string& pass);
        void set_response(const std::string& _resp);
        void set_info(const struct sockaddr_in& info);
        void set_buff(const std::string& _buff, bool append = false);
};


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
