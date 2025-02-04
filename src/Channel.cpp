/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 11:13:49 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/30 11:13:49 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

// Default constructor
Channel::Channel() : name(""), topic(""), password("") {
    std::cout << formatServerMessage("DEBUG", "Creating new empty channel") << std::endl;
}

// Parameterized constructors
Channel::Channel(const std::string& name) : name(name), topic(""), password("") {
    std::ostringstream debug;
    debug << "Creating new channel: " << name;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}

Channel::Channel(const std::string& name, const std::string& topic) : name(name), topic(topic), password("") {
    std::ostringstream debug;
    debug << "Creating new channel: " << name << " with topic: " << topic;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}

Channel::Channel(const std::string& name, const std::string& topic, const std::string& password) 
    : name(name), topic(topic), password(password) {
    std::ostringstream debug;
    debug << "Creating new channel: " << name << " with topic: " << topic << " and password";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}

// Destructor
Channel::~Channel() {
    std::cout << formatServerMessage("DEBUG", "Destroying channel: " + name) << std::endl;
}

// Getters
const std::string& Channel::getName() const { return name; }
const std::string& Channel::getTopic() const { return topic; }
const std::string& Channel::getPassword() const { return password; }
const std::vector<Client>& Channel::getMembers() const { return members; }

// Setters
void Channel::setName(const std::string& n) { name = n; }
void Channel::setTopic(const std::string& t) { topic = t; }
void Channel::setPassword(const std::string& p) { password = p; }

// Member management methods
void Channel::addMember(const Client& member) {
    for (std::vector<Client>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (it->getNickName() == member.getNickName()) {
            std::ostringstream warning;
            warning << "User " << member.getNickName() << " is already in channel " << name;
            std::cout << formatServerMessage("DEBUG", warning.str()) << std::endl;
            return;
        }
    }
    members.push_back(member);
    std::ostringstream success;
    success << "Added " << member.getNickName() << " to channel " << name << " (Total members: " << members.size() << ")";
    std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
}

bool Channel::removeMember(const std::string& nickname) {
    for (std::vector<Client>::iterator it = members.begin(); it != members.end(); ++it) {
        if (it->getNickName() == nickname) {
            members.erase(it);
            std::ostringstream success;
            success << "Removed " << nickname << " from channel " << name << " (Remaining members: " << members.size() << ")";
            std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
            return true;
        }
    }
    std::ostringstream error;
    error << "User " << nickname << " not found in channel " << name;
    std::cout << formatServerMessage("ERROR", error.str()) << std::endl;
    return false;
}

bool Channel::isMember(const std::string& nickname) const {
    for (std::vector<Client>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (it->getNickName() == nickname) {
            return true;
        }
    }
    return false;
}

std::string Channel::getMembersList() const {
    std::ostringstream memberList;
    for (std::vector<Client>::const_iterator it = members.begin(); it != members.end(); ++it) {
        if (it != members.begin())
            memberList << " ";
        memberList << it->getNickName();
    }
    return memberList.str();
}

void Channel::broadcast(const std::string& message, const std::string& except_nick) {
    std::ostringstream debug;
    debug << "Broadcasting to channel " << name << ": " << (message.length() > 50 ? message.substr(0, 50) + "..." : message);
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    for (std::vector<Client>::iterator it = members.begin(); it != members.end(); ++it) {
        if (it->getNickName() != except_nick) {
            it->setResponse(message);
        }
    }
}

void Channel::clearMembers() {
    members.clear();
    std::ostringstream info;
    info << "Cleared all members from channel " << name;
    std::cout << formatServerMessage("INFO", info.str()) << std::endl;
}

// Utility methods
bool Channel::hasPassword() const {
    return !password.empty();
}

bool Channel::checkPassword(const std::string& pass) const {
    return password == pass;
}

void Channel::printInfo() const {
    std::ostringstream info;
    info << "Channel Info: [Name: " << name << ", Topic: " << topic << ", Password: " << (password.empty() ? "None" : "Set") << ", Members: " << members.size() << "]";
    std::cout << formatServerMessage("INFO", info.str()) << std::endl;
}
