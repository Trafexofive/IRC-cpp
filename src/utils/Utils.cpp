/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 16:31:50 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/02/02 19:38:29 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include "../../inc/Helpers.hpp"
#include "../../inc/Client.hpp"
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

#define SERVER_NAME "WeUseArch"
#define SERVER_HOST "morpheus-server.ddns.net"

#define CRLF "\r\n"

/* ************************************************************************** */
/*                       SERVER UTILS                                         */
/* ************************************************************************** */
std::string formatServerMessage(const std::string& type, const std::string& message)
{
    std::ostringstream oss;
    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    oss << BOLD << "[" << timestamp << "] " << RESET;
    
    if (type == "DEBUG")
        oss << YELLOW << "DEBUG :: " << RESET;
    else if (type == "ERROR")
        oss << RED << "ERROR :: " << RESET;
    else if (type == "WARNING")
        oss << YELLOW << BOLD << "WARNING :: " << RESET;
    else if (type == "INFO")
        oss << GREEN << "INFO  :: " << RESET;
    else if (type == "CLIENT")
        oss << CYAN << "CLIENT :: " << RESET;
    else if (type == "SERVER")
        oss << MAGENTA << "SERVER :: " << RESET;
    else if (type == "FATAL")
        oss << RED << BOLD << "FATAL :: " << RESET;
    
    oss << message;
    return oss.str();

}

void printServerMessage(const std::string& type, const std::string& message)
{
    std::cout << formatServerMessage(type, message) << std::endl;
}


// Clients

/* ************************************************************************** */
/*                       CLIENT UTILS                                         */
/* ************************************************************************** */

// std::string formatResponse(const std::string& code, const std::string& message)
// {
//     std::ostringstream oss;
//     oss << ":WeUseArch " << code << " " << message;
//     return oss.str();
// }




// Basic formatting helper function (not a macro)
std::string formatResponse(const std::string& code, const std::string& message)
{
        return ":WeUseArch.localhost " + code + " " + message + "\r\n";
}

std::string formatResponseSource(const std::string& source, const std::string& code, const std::string& message)
{
    return ":" + source + " " + code + " " + message + "\r\n";
}

std::string formatResponseServerCommand(std::vector<std::string> &args)
{
    std::ostringstream oss;
    std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(oss, " "));

    return ":WeUseArch "  + oss.str() + CRLF;
}

std::string formatResponseClientCommand(const std::string& source, const std::string& command, const std::string& message)
{
    return ":" + source + " " + command + " " + message + "\r\n";
}


