#include "../../inc/Client.hpp"
#include <algorithm>
#include <sstream>

_client::_client() : 
    fd_client(-1), 
    auth(false), 
    connected(false) 
{
    std::cout << formatServerMessage("DEBUG", "Creating new empty client instance") << std::endl;
}

_client::_client(int fd, struct sockaddr_in ddr) :
    fd_client(fd),
    auth(false),
    connected(false),
    client_infos(ddr)
{
    std::ostringstream debug;
    debug << "Creating new client instance with fd(" << fd << ") and IP(" 
          << inet_ntoa(ddr.sin_addr) << ":" << ntohs(ddr.sin_port) << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}

void _client::authenticate() 
{
    std::ostringstream debug;
    debug << "Attempting authentication for client fd(" << fd_client << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    if (!password.empty() && !nickname.empty() && !fullname.empty()) 
    {
        auth = true;
        connected = true;
        
        std::ostringstream success;
        success << "Authentication successful for " << nickname 
                << " (fd: " << fd_client << ")";
        std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
    }
    else
    {
        std::ostringstream error;
        error << "Authentication failed - Missing: "
              << (password.empty() ? "password " : "")
              << (nickname.empty() ? "nickname " : "")
              << (fullname.empty() ? "fullname" : "");
        std::cout << formatServerMessage("ERROR", error.str()) << std::endl;
    }
}

void _client::set_buff(const std::string& _buff, bool append) 
{
    std::ostringstream debug;
    debug << "Setting buffer for client fd(" << fd_client << "): " 
          << (_buff.length() > 50 ? _buff.substr(0, 50) + "..." : _buff)
          << (append ? " (appending)" : " (replacing)");
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    if (append)
        buff += _buff;
    else
        buff = _buff;
}

void _client::set_response(const std::string& _response)
{
    std::ostringstream debug;
    debug << "Sending response to client fd(" << fd_client << "): " 
          << (_response.length() > 50 ? _response.substr(0, 50) + "..." : _response);
    std::cout << formatServerMessage("SERVER", debug.str()) << std::endl;

    std::string full_response = _response + "\r\n";
    send(fd_client, full_response.c_str(), full_response.length(), 0);
}

void _client::clear() 
{
    std::ostringstream debug;
    debug << "Clearing client data for fd(" << fd_client << ")";
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    auth = false;
    connected = false;
    ip_addr.clear();
    fullname.clear();
    nickname.clear();
    password.clear();
    buff.clear();
    response.clear();
}

// Channel implementation
channel::channel() : 
    name(""), 
    topic(""), 
    password("") 
{
    std::cout << formatServerMessage("DEBUG", "Creating new empty channel") << std::endl;
}

channel::channel(const std::string& n) : 
    name(n), 
    topic(""), 
    password("") 
{
    std::ostringstream debug;
    debug << "Creating new channel: " << n;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;
}

void channel::addMember(const _client& member) 
{
    std::ostringstream debug;
    debug << "Attempting to add " << member.get_nick() << " to channel " << name;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    for (std::vector<_client>::const_iterator it = members.begin(); 
         it != members.end(); ++it) 
    {
        if (it->get_nick() == member.get_nick())
        {
            std::ostringstream warning;
            warning << "User " << member.get_nick() << " is already in channel " << name;
            std::cout << formatServerMessage("DEBUG", warning.str()) << std::endl;
            return;
        }
    }
    
    members.push_back(member);
    
    std::ostringstream success;
    success << "Added " << member.get_nick() << " to channel " << name 
            << " (Total members: " << members.size() << ")";
    std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
}

bool channel::removeMember(const std::string& nickname) 
{
    std::ostringstream debug;
    debug << "Attempting to remove " << nickname << " from channel " << name;
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    for (std::vector<_client>::iterator it = members.begin(); 
         it != members.end(); ++it) 
    {
        if (it->get_nick() == nickname) 
        {
            members.erase(it);
            
            std::ostringstream success;
            success << "Removed " << nickname << " from channel " << name 
                    << " (Remaining members: " << members.size() << ")";
            std::cout << formatServerMessage("DEBUG", success.str()) << std::endl;
            return true;
        }
    }

    std::ostringstream error;
    error << "User " << nickname << " not found in channel " << name;
    std::cout << formatServerMessage("ERROR", error.str()) << std::endl;
    return false;
}

std::string channel::getMembersList() const
{
    std::ostringstream memberList;
    for (std::vector<_client>::const_iterator it = members.begin();
         it != members.end(); ++it)
    {
        if (it != members.begin())
            memberList << " ";
        memberList << it->get_nick();
    }
    return memberList.str();
}

void channel::broadcast(const std::string& message, const std::string& except_nick)
{
    std::ostringstream debug;
    debug << "Broadcasting to channel " << name << ": " 
          << (message.length() > 50 ? message.substr(0, 50) + "..." : message);
    std::cout << formatServerMessage("DEBUG", debug.str()) << std::endl;

    for (std::vector<_client>::iterator it = members.begin();
         it != members.end(); ++it)
    {
        if (it->get_nick() != except_nick)
            it->set_response(message);
    }
}
