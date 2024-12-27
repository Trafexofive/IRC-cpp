/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 04:27:11 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 04:27:11 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/core_server.hpp"
#include <cstdlib>

// Debug colors
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void CoreServer::create_socket()
{
    std::cout << GREEN "[DEBUG] Creating socket..." RESET << std::endl;
    
    ServData._socket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServData._socket < 0)
    {
        std::cerr << RED "[ERROR] Socket creation failed" RESET << std::endl;
        exit(1);
    }
    
    std::cout << GREEN "[DEBUG] Socket created: " << ServData._socket << RESET << std::endl;
    
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
    
    std::cout << GREEN "[DEBUG] Socket setup complete" RESET << std::endl;
}

void CoreServer::start_listening()
{
    std::cout << YELLOW "[DEBUG] Binding socket..." RESET << std::endl;
    
    if (bind(ServData._socket, (struct sockaddr*)&ServData.ServAddr, sizeof(ServData.ServAddr)) < 0)
    {
        std::cerr << RED "[ERROR] Binding failed" RESET << std::endl;
        exit(1);
    }
    
    std::cout << YELLOW "[DEBUG] Starting to listen..." RESET << std::endl;
    
    if (listen(ServData._socket, 5) < 0)
    {
        std::cerr << RED "[ERROR] Listen failed" RESET << std::endl;
        exit(1);
    }
    std::cout << GREEN "[SUCCESS] Server listening on port:" << ServData.Port << RESET << std::endl;
}

void CoreServer::start_server()
{
    std::cout << GREEN "[DEBUG] Server main loop starting..." RESET << std::endl;
    
    while (1)
    {
        int ret = poll(&fds[0], fds.size(), -1);
        if (ret < 0)
        {
            std::cerr << RED "[ERROR] Poll failed" RESET << std::endl;
            break;
        }
        
        std::cout << YELLOW "[DEBUG] Active fds: " << fds.size() << RESET << std::endl;
        
        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == ServData._socket)
                {
                    std::cout << GREEN "[DEBUG] New connection incoming" RESET << std::endl;
                    WelcomeClient();
                }
                else
                {
                    std::cout << YELLOW "[DEBUG] Data from client " << fds[i].fd << RESET << std::endl;
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
    std::cout << YELLOW "[DEBUG] Validating port: " << port << RESET << std::endl;
    
    if (!isStringDigits(port))
    {
        std::cout << RED "[ERROR] Port contains non-digits" RESET << std::endl;
        return false;
    }

    char* end;
    long val = std::strtol(port.c_str(), &end, 10);
    
    if (*end != '\0' || val <= 0 || val > 65535)
    {
        std::cout << RED "[ERROR] Port out of range" RESET << std::endl;
        return false;
    }
        
    _tport = static_cast<int>(val);
    std::cout << GREEN "[DEBUG] Port valid: " << _tport << RESET << std::endl;
    return true;
}

bool IsValidPass(std::string _pass, std::string& passwd)
{
    std::cout << YELLOW "[DEBUG] Validating password" RESET << std::endl;
    
    if (_pass.length() < 8)
    {
        std::cout << RED "[ERROR] Password too short" RESET << std::endl;
        return false;
    }

    bool Lower = false, Upper = false, Digit = false, Special = false;
    
    for (std::string::iterator it = _pass.begin(); it != _pass.end(); ++it)
    {
        if (islower(*it)) Lower = true;
        else if (isupper(*it)) Upper = true;
        else if (isdigit(*it)) Digit = true;
        else if (ispunct(*it)) Special = true;
    }
    
    if (!(Lower && Upper && Digit && Special))
    {
        std::cout << RED "[ERROR] Password missing required characters" RESET << std::endl;
        return false;
    }
        
    passwd = _pass;
    std::cout << GREEN "[DEBUG] Password valid" RESET << std::endl;
    return true;
}

CoreServer::CoreServer(std::string port, std::string password)
{
    std::cout << GREEN "[DEBUG] Initializing server..." RESET << std::endl;
    
    if (!IsValidPort(port, ServData.Port))
    {
        std::cerr << RED "[ERROR] Invalid port" RESET << std::endl;
        return;
    }
    if (!IsValidPass(password, ServData.Passwd))
    {
        std::cerr << RED "[ERROR] Invalid password requirements:" RESET << std::endl;
        std::cout << "- At least 8 characters" << std::endl;
        std::cout << "- At least 1 lowercase letter (a-z)" << std::endl;
        std::cout << "- At least 1 uppercase letter (A-Z)" << std::endl;
        std::cout << "- At least 1 digit (0-9)" << std::endl;
        std::cout << "- At least 1 special character" << std::endl;
        return;
    }
    
    std::cout << YELLOW "[DEBUG] Registering commands..." RESET << std::endl;
    commands[CMD_NICK] = &CoreServer::cmdNick;
    commands[CMD_USER] = &CoreServer::cmdUser;
    commands[CMD_JOIN] = &CoreServer::cmdJoin;
    commands[CMD_PRIVMSG] = &CoreServer::cmdPrivmsg;
    commands[CMD_PASS] = &CoreServer::cmdPass;
    
    create_socket();
    start_listening();
    start_server();
}

int main(int ac, char** av)
{
    std::cout << GREEN "[DEBUG] Starting IRC server" RESET << std::endl;
    
    if (ac != 3 || !av[1] || !av[2])
    {
        std::cerr << RED "[ERROR] Usage: " << av[0] << " <port> <password>" RESET << std::endl;
        return 1;
    }
    
    std::string port(av[1]);
    std::string passwd(av[2]);
    std::cout << YELLOW "[DEBUG] Attempting to start server on port " << port << RESET << std::endl;
    CoreServer IrcServ(port, passwd);
    return 0;
}
