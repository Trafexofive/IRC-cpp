/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 03:42:57 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 03:42:57 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client.hpp"
#include <algorithm>

// _client implementation
_client::_client() : 
    fd_client(-1), 
    auth(false), 
    connected(false) 
{
}

_client::_client(int fd, struct sockaddr_in ddr) :
    fd_client(fd),
    auth(false),
    connected(false),
    client_infos(ddr)
{
}

void _client::authenticate() 
{
    if (!password.empty() && !nickname.empty() && !fullname.empty()) 
    {
        auth = true;
        connected = true;
    }
}

void _client::set_buff(const std::string& _buff, bool append) 
{
    if (append)
        buff += _buff;
    else
        buff = _buff;
}

void _client::clear() 
{
    auth = false;
    connected = false;
    ip_addr.clear();
    fullname.clear();
    nickname.clear();
    password.clear();
    buff.clear();
    response.clear();
}

// channel implementation
channel::channel() : 
    name(""), 
    topic(""), 
    password("") 
{
}

channel::channel(const std::string& n) : 
    name(n), 
    topic(""), 
    password("") 
{
}

void channel::addMember(const _client& member) 
{
    for (std::vector<_client>::const_iterator it = members.begin(); 
         it != members.end(); ++it) 
    {
        if (it->get_nick() == member.get_nick())
            return;
    }
    members.push_back(member);
}

bool channel::removeMember(const std::string& nickname) 
{
    for (std::vector<_client>::iterator it = members.begin(); 
         it != members.end(); ++it) 
    {
        if (it->get_nick() == nickname) 
        {
            members.erase(it);
            return true;
        }
    }
    return false;
}
