/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Trafexofive <mboutuil@student.42.fr>       +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024-12-27 03:16:38 by Trafexofive       #+#    #+#           */
/*   Updated: 2024-12-27 03:16:38 by Trafexofive      ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/core_server.hpp"
#include <cstdlib>

void CoreServer::create_socket()
{
    ServData._socket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServData._socket < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(1);
    }
    int flags = fcntl(ServData._socket, F_GETFL, 0);
    fcntl(ServData._socket, F_SETFL, flags | O_NONBLOCK);
    int optval = 1;
    setsockopt(ServData._socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    ServData.ServAddr.sin_family = AF_INET;
    ServData.ServAddr.sin_port = htons(ServData.Port);
    ServData.ServAddr.sin_addr.s_addr = INADDR_ANY;
    struct pollfd _fd;
    _fd.fd = ServData._socket;
    _fd.events = POLLIN;
    fds.push_back(_fd);
}

void CoreServer::start_listening()
{
    if (bind(ServData._socket, (struct sockaddr*)&ServData.ServAddr, sizeof(ServData.ServAddr)) < 0)
    {
        std::cout << "error biding the socket" << std::endl;
        exit(1);
    }
    if (listen(ServData._socket, 5) < 0)
    {
        std::cout << "error" << std::endl;
        exit(1);
    }
    std::cout << "start listening on port:" << ServData.Port << "..." << std::endl;
}

void CoreServer::start_server()
{
    while (1)
    {
        int ret = poll(&fds[0], fds.size(), -1);
        if (ret < 0)
        {
            std::cout << "poll error" << std::endl;
            break;
        }
        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == ServData._socket)
                {
                    WelcomeClient();
                }
                else
                {
                    ReadEvent(fds[i].fd);
                }
            }
        }
    }
}

bool isStringDigits(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!isdigit(*it))
            return false;
    }
    return true;
}

bool IsValidPort(std::string port, int& _tport)
{
    if (!isStringDigits(port))
        return false;

    char* end;
    long val = std::strtol(port.c_str(), &end, 10);
    
    if (*end != '\0' || val <= 0 || val > 65535)
        return false;
        
    _tport = static_cast<int>(val);
    return true;
}

bool IsValidPass(std::string _pass, std::string& passwd)
{
    if (_pass.length() < 8)
        return false;

    bool Lower = false, Upper = false, Digit = false, Special = false;
    
    for (std::string::iterator it = _pass.begin(); it != _pass.end(); ++it)
    {
        if (islower(*it)) Lower = true;
        else if (isupper(*it)) Upper = true;
        else if (isdigit(*it)) Digit = true;
        else if (ispunct(*it)) Special = true;
    }
    
    if (!(Lower && Upper && Digit && Special))
        return false;
        
    passwd = _pass;
    return true;
}

CoreServer::CoreServer(std::string port, std::string password)
{
    if (!IsValidPort(port, ServData.Port))
    {
        std::cout << "ENTER VALID PORT !!" << std::endl;
        return;
    }
    if (!IsValidPass(password, ServData.Passwd))
    {
        std::cout << "ENTER VALID PASSWORD !!" << std::endl;
        std::cout << "At least 1 lowercase letter (a-z) !!" << std::endl;
        std::cout << "At least 1 uppercase letter (A-Z) !!" << std::endl;
        std::cout << "At least 1 digit (0-9) !!" << std::endl;
        std::cout << "At least 1 special character (e.g., !, @, #, $, etc.) !!" << std::endl;
        return;
    }
    
    commands[CMD_NICK] = &CoreServer::cmdNick;
    commands[CMD_USER] = &CoreServer::cmdUser;
    commands[CMD_JOIN] = &CoreServer::cmdJoin;
    commands[CMD_PRIVMSG] = &CoreServer::cmdPrivmsg;
    commands[CMD_PASS] = &CoreServer::cmdPass;  // Added PASS command
    
    create_socket();
    start_listening();
    start_server();
}

int main(int ac, char** av)
{
    if (ac != 3 || !av[1] || !av[2])
    {
        std::cout << "ENTER VALID PARAMETERS !!" << std::endl;
        return 1;
    }
    
    std::string port(av[1]);
    std::string passwd(av[2]);
    CoreServer IrcServ(port, passwd);
    return 0;
}
