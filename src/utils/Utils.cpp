/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/31 16:31:50 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/31 16:31:50 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include "../../inc/Helpers.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>

// should be removed.
// std::string formatServerMessage(const ServerData& serverData, const std::string& type, const std::string& message)
// {
//     // Determine the logging level based on the type
//     LEVEL messageLevel;
//     if (type == "DEBUG")
//         messageLevel = DEBUG;
//     else if (type == "ERROR")
//         messageLevel = ERROR;
//     else if (type == "WARNING")
//         messageLevel = WARNING;
//     else if (type == "INFO")
//         messageLevel = INFO;
//     else if (type == "CLIENT")
//         messageLevel = INFO;
//     else if (type == "SERVER")
//         messageLevel = INFO;
//
//     // Check if the logging level is appropriate
//     if (messageLevel < serverData._infoLevel) {
//         return ""; // Do not log the message if the level is lower than the current logging level
//                    // honestly this should log regardless.
//     }
//     return "";
// }


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
        oss << GREEN << "INFO :: " << RESET;
    else if (type == "CLIENT")
        oss << CYAN << "CLIENT << " << RESET;
    else if (type == "SERVER")
        oss << MAGENTA << "SERVER >> " << RESET;
    else if (type == "FATAL")
        oss << RED << BOLD << "FATAL :: " << RESET;
    
    oss << message;
    return oss.str();

}

