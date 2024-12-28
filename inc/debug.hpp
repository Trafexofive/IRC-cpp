/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 04:24:51 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 04:24:51 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_DEBUG_HPP
#define CLIENT_DEBUG_HPP

#include <iostream>
#include <string>
#include <ctime>


//in progress...

class ClientDebug
{
private:
    static const char* GREEN;
    static const char* RED;
    static const char* YELLOW;
    static const char* RESET;

    static std::string getTimestamp()
    {
        time_t now = time(NULL);
        char buffer[26];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&now));
        return std::string(buffer);
    }

public:
    static void logAuth(int fd, const std::string& nickname, bool success)
    {
        std::cout << "[" << getTimestamp() << "] Client " << fd 
                 << (success ? (GREEN " AUTH SUCCESS: ") : (RED " AUTH FAILED: "))
                 << nickname << RESET << std::endl;
    }

    static void logMessage(int fd, const std::string& nickname, const std::string& message)
    {
        std::cout << "[" << getTimestamp() << "] " << YELLOW 
                 << nickname << "(" << fd << "): " 
                 << message << RESET << std::endl;
    }

    static void logConnection(int fd, const std::string& ip)
    {
        std::cout << "[" << getTimestamp() << "] " << GREEN 
                 << "New connection " << fd << " from " << ip 
                 << RESET << std::endl;
    }

    static void logDisconnection(int fd, const std::string& nickname)
    {
        std::cout << "[" << getTimestamp() << "] " << RED 
                 << "Client disconnected: " << nickname << "(" << fd << ")"
                 << RESET << std::endl;
    }
};

// Initialize static members
const char* ClientDebug::GREEN = "\033[32m";
const char* ClientDebug::RED = "\033[31m";
const char* ClientDebug::YELLOW = "\033[33m";
const char* ClientDebug::RESET = "\033[0m";

#endif
