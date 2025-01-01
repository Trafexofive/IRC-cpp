/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 10:57:28 by mlamkadm          #+#    #+#             */
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
    passWord(""),
    buff(""),
    response("")
{
    std::cout << formatServerMessage("DEBUG", "Creating new empty client instance") << std::endl;
}

// Parameterized constructor
Client::Client(int fd, struct sockaddr_in ddr) :
    fdClient(fd),
    auth(false),
    connected(false),
    clientInfos(ddr),
    ipAddr(""),
    fullName(""),
    nickName(""),
    passWord(""),
    buff(""),
    response("")
{
    std::ostringstream debug;
    debug << "Creating new client instance with fd(" << fd << ") and IP(" 
          << inet_ntoa(ddr.sin_addr) << ":" << ntohs(ddr.sin_port) << ")";
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
const std::string& Client::getBuff() const { return buff; }
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
void Client::setResponse(const std::string& response) { this->response = response; }
void Client::setClientInfos(const struct sockaddr_in& info) { clientInfos = info; }
void Client::setBuff(const std::string& _buff, bool append) 
{
    std::ostringstream debug;
    debug << "Setting buffer for client fd(" << fdClient << "): " 
          << (_buff.length() > 50 ? _buff.substr(0, 50) + "..." : _buff)
          << (append ? " (appending)" : " (replacing)");
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    if (append)
        buff += _buff;
    else
        buff = _buff;
}

// Class specific methods
void Client::sendMessage(const std::string& message)
{
    std::ostringstream debug;
    debug << "Sending message to client fd(" << fdClient << "): " 
          << (message.length() > 50 ? message.substr(0, 50) + "..." : message);
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    // Implementation to send the message to the client
}

std::string Client::receiveMessage()
{
    std::ostringstream debug;
    debug << "Receiving message from client fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    // Implementation to receive a message from the client
    return "";
}

void Client::disconnect()
{
    std::ostringstream debug;
    debug << "Disconnecting client fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    // Implementation to disconnect the client
}

bool Client::isAuthenticated() const
{
    return auth;
}

void Client::updateNickName(const std::string& newNickName)
{
    std::ostringstream debug;
    debug << "Updating nickname for client fd(" << fdClient << ") to " << newNickName;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    nickName = newNickName;
}

void Client::logActivity(const std::string& activity)
{
    std::ostringstream debug;
    debug << "Logging activity for client fd(" << fdClient << "): " << activity;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    // Implementation to log activity
}

void Client::resetPassword(const std::string& newPassword)
{
    std::ostringstream debug;
    debug << "Resetting password for client fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    passWord = newPassword;
}

void Client::authenticate() 
{
    std::ostringstream debug;
    debug << "Attempting authentication for client fd(" << fdClient << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    if (!passWord.empty() && !nickName.empty() && !fullName.empty()) 
    {
        auth = true;
        connected = true;
        
        // Set welcome messages
        std::string welcomeMsg = ":server 001 " + nickName + " :Welcome to the IRC Network";
        setResponse(welcomeMsg);
        
        std::ostringstream success;
        success << "Authentication successful for " << nickName 
                << " (fd: " << fdClient << ")";
        std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
    }
    else
    {
        std::ostringstream error;
        error << "Authentication failed - Missing: "
              << (passWord.empty() ? "password " : "")
              << (nickName.empty() ? "nickname " : "")
              << (fullName.empty() ? "fullname" : "");
        std::cout << formatServerMessage("ERROR", error.str()) << std::endl;
    }
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
    buff.clear();
    response.clear();
}

void Client::clearResponse()
{
    response.clear();
}
