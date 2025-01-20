/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */ /*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 21:33:01 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/29 21:33:01 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <string>
#include <vector>
#include "Client.hpp"


typedef struct {
    enum TYPE{
        PUBLIC,
        PRIVATE,
        UNKNOWN
    };
    int state;
}
CHANNEL;

class Channel {
private:
    std::string name;
    std::string topic;
    std::string password;

    std::vector<Client> members;
    CHANNEL _type;


public:
    // Default constructor
    Channel();
    // Parameterized constructors
    Channel(const std::string& name);
    Channel(const std::string& name, const std::string& topic);
    Channel(const std::string& name, const std::string& topic, const std::string& password);
    // Destructor
    ~Channel();

    // Getters
    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::string& getPassword() const;
    const std::vector<Client>& getMembers() const;
    std::string getMembersList() const;
    const std::string getType() const
    {
        if (_type.state == CHANNEL::PUBLIC)
            return "PUBLIC";
        else if (_type.state == CHANNEL::PRIVATE)
            return "PRIVATE";
        else
            return "UNKNOWN";
    }

    // Setters
    void setName(const std::string& n);
    void setTopic(const std::string& t);
    void setPassword(const std::string& p);

    // Member management methods
    void addMember(const Client& member);
    bool removeMember(const std::string& nickname);
    bool isMember(const std::string& nickname) const;

    // Channel-specific methods
    void clearMembers();

    // Utility methods
    bool hasPassword() const;
    bool checkPassword(const std::string& pass) const;

    // General methods
    void broadcast(const std::string& message);

};

// helper functions

bool    isChannel(const std::string& name);
Channel& getChannel(const std::string& name, std::vector<Channel>& channels);
void removeClientFromChannel(Client& client, Channel& channel) ;
void handleInvalidChannel(Client& client, const std::string& channelName) ;
void handlePartSuccess(Client& client, const std::string& channelName) ;
std::string getChannelsString(const std::vector<Channel>& channels);



#endif // CHANNEL_HPP
