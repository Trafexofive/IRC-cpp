/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */ /*   Created: 2024/12/27 10:57:28 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 10:57:28 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client.hpp"
#include "../../inc/Channel.hpp"

// Default constructor
Client::Client() : 
    fdClient(-1), 
    auth(false), 
    connected(false),
    clientInfos(),
    ipAddr(""),
    fullName(""),
    nickName(""),
    realName(""),
    passWord(""),
    response(""),
    source("")
{
    std::cout << formatServerMessage("DEBUG", "Creating new empty client instance") << std::endl;
}

//copy constructor
Client::Client(const Client &client) : 
    fdClient(client.fdClient), 
    auth(client.auth), 
    connected(client.connected),
    clientInfos(client.clientInfos),
    ipAddr(client.ipAddr),
    fullName(client.fullName),
    nickName(client.nickName),
    realName(client.realName),
    passWord(client.passWord),
    response(client.response),
    source(client.source)
{
    std::ostringstream debug;
    debug << "Creating new client instance for fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}

// assignment operator



// Parameterized constructor
Client::Client(int fd, struct sockaddr_in addr) : 
    fdClient(fd), 
    auth(false), 
    connected(false),
    clientInfos(addr),
    ipAddr(inet_ntoa(addr.sin_addr)),
    fullName(""),
    nickName(""),
    realName(""),
    passWord(""),
    response("")
{
    std::ostringstream debug;
    debug << "Creating new client instance for fd(" << fd << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}
// Getters
int Client::getFd() const { return fdClient; }
bool Client::getAuth() const { return auth; }
bool Client::getConnected() const { return connected; }
const std::string& Client::getIpAddr() const { return ipAddr; }
const std::string& Client::getFullName() const { return fullName; }
const std::string& Client::getNickName() const { return nickName; }
const std::string& Client::getPassWord() const { return passWord; }
const std::string& Client::getResponse() const { return response; }
const struct sockaddr_in& Client::getClientInfos() const { return clientInfos; }

// Setters
void Client::setFd(int fd) { fdClient = fd; }
void Client::setAuth(bool auth) { this->auth = auth; }
void Client::setConnected(bool connected) { this->connected = connected; }
void Client::setIpAddr(const std::string& ip) { ipAddr = ip; }
void Client::setFullName(const std::string& user) { fullName = user; }
void Client::setNickName(const std::string& nick) { nickName = nick; }
void Client::setPassWord(const std::string& pass) { passWord = pass; }
void Client::setResponse(const std::string& response) {
    if (response.empty())
        return;
    else
        this->response += response;
}

void Client::setClientInfos(const struct sockaddr_in& info) { clientInfos = info; }

void Client::disconnect()
{
    std::ostringstream debug;
    debug << "Disconnecting client fdr(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
    close(fdClient);
    clear();
}

bool Client::isAuthenticated() const
{
    return auth;
}

void Client::clear() 
{
    std::ostringstream debug;
    debug << "Clearing client data for fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    auth = false;
    connected = false;
    ipAddr.clear();
    fullName.clear();
    nickName.clear();
    passWord.clear();
    response.clear();
}

void Client::clearResponse()
{
    response.clear();
}

Client::~Client()
{

    std::ostringstream debug;
    debug << "Destroying client instance for fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
    // this->disconnect();
}
