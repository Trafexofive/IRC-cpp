/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
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

#include "Client.hpp"

class channel
{
    private:
        std::string name;
        std::string topic;
        std::string password;
        std::vector<_client> members;

        
    public:
        channel();
        channel(const std::string& n);
        ~channel() {}

        // Getters
        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::string& getPassword() const;
        const std::vector<_client>& getMembers() const;
        std::string getMembersList() const;
        
        // Setters
        void setName(const std::string& n);
        void setTopic(const std::string& t);
        void setPassword(const std::string& p);
        
        void addMember(const _client& member);
        bool removeMember(const std::string& nickname);
        void broadcast(const std::string& message, const std::string& except_nick = "");
};

#endif
