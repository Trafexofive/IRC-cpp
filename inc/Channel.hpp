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
        // LOCAL, // if this is allowed Ill handle it.
        EMPTY,
        UNKNOWN
    };
    int state;

    bool inviteMode;
    bool topicMode;
    bool operatorMode;
    bool keyMode;

}

CHANNEL;

class Channel {
private:
    std::string name;
    std::string topic;
    std::string password;

    std::vector<Client *> members;
    // std::vector<Client> &operators;

    CHANNEL _type;


public:
    // Default constructor
    Channel();
    // Parameterized constructors
    Channel(const std::string& name, Client* client);
    Channel(const std::string& name, const std::string& topic, Client* client);
    Channel(const std::string& name, const std::string& topic, const std::string& password, Client* client);
    // Destructor
    ~Channel();

    // Channel (const Channel& obj, const Client& client)
    // {
    //     name = obj.name;
    //     topic = obj.topic;
    //     password = obj.password;
    //     members = obj.members;
    //     _type = obj._type;
    //
    //     members.push_back(client);
    // }
    // const Channel& operator=(const Channel& obj)
    // {
    //     
    //
    //
    // }

    // Getters
    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::string& getPassword() const;
    const std::vector<Client*>& getMembers() const;
    const std::string getState() const
    {
        if (_type.state == CHANNEL::PUBLIC)
            return "PUBLIC";
        else if (_type.state == CHANNEL::PRIVATE)
            return "PRIVATE";
        // else if (_type.state == CHANNEL::LOCAL)
        //     return "LOCAL";
        else if (_type.state == CHANNEL::EMPTY)
            return "EMPTY";
        else
            return "UNKNOWN";
    }

    // Setters
    void setName(const std::string& n);
    void setTopic(const std::string& t);
    void setPassword(const std::string& p);
    void setState(const std::string& s)
    {
        if (s == "PUBLIC")
            _type.state = CHANNEL::PUBLIC;
        else if (s == "PRIVATE")
            _type.state = CHANNEL::PRIVATE;
        // else if (s == "LOCAL")
        //     _type.state = CHANNEL::LOCAL;
        else if (s == "EMPTY")
            _type.state = CHANNEL::EMPTY;
        else
            _type.state = CHANNEL::UNKNOWN;
    }

    // Member management methods
    bool removeMember(const std::string& nickname);

    void addMember(Client* member);
    bool removeMember(Client* obj);
    bool isMember(const std::string& nickname) const;

    // Channel-specific methods
    void clearMembers();

    // Utility methods
    bool hasPassword() const; // should be removed
    bool checkPassword(const std::string& pass) const; //should be renamed to validate password

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
