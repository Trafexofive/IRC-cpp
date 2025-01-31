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
#include "../../inc/Client.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

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

std::string numberToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string numberToString(bool value) {
    std::ostringstream oss;
    oss << (value ? "true" : "false");
    return oss.str();
}

// static std::string getStatusString(STATUS::TYPE satus) {
//
// }

void Client::printClientInfo() {
    std::cout << formatServerMessage("DEBUG", "----------------------------------\nClient: Fd=" + numberToString(fdClient) + 
              " IP:" + ipAddr + " Name:" + fullName + " | Nick:" + nickName + 
              " RealName:" + realName + " | Pass:" + passWord + 
              " | Response:" + response + " | target:" + _target + 
              " | Type=" + numberToString(_status.state)) << std::endl;
}
