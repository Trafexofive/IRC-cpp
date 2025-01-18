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

// Enhanced helper function to print a field with server message formatting
void printField(const std::string& field, const std::string& value, int col1Width, int col2Width) {
    std::ostringstream row;
    row << "| " << std::left << std::setw(col1Width) << field 
        << " | " << std::setw(col2Width) << value << " |";
    std::cout << formatServerMessage("DEBUG", row.str()) << std::endl;
}

void Client::printClientInfo()
{
    // should be MACROS with control @ Makefile
    const int COL1_WIDTH = 15; // Width for the field name column
    const int COL2_WIDTH = 29; // Width for the field value column
    
    std::cout << formatServerMessage("DEBUG", "Client Table:") << std::endl;
    std::cout << formatServerMessage("DEBUG", "+-----------------+-------------------------------+") << std::endl;
    
    std::ostringstream header;
    header << "| " << std::left << std::setw(COL1_WIDTH) << "Field" 
           << " | " << std::setw(COL2_WIDTH) << "Value" << " |";
    std::cout << formatServerMessage("DEBUG", header.str()) << std::endl;
    
    std::cout << formatServerMessage("DEBUG", "+-----------------+-------------------------------+") << std::endl;
    
    // Print each field using the enhanced function
    printField("fdClient", numberToString(fdClient), COL1_WIDTH, COL2_WIDTH);
    printField("auth", numberToString(auth), COL1_WIDTH, COL2_WIDTH);
    printField("connected", numberToString(connected), COL1_WIDTH, COL2_WIDTH);
    printField("ipAddr", ipAddr, COL1_WIDTH, COL2_WIDTH);
    printField("fullName", fullName, COL1_WIDTH, COL2_WIDTH);
    printField("nickName", nickName, COL1_WIDTH, COL2_WIDTH);
    printField("realName", realName, COL1_WIDTH, COL2_WIDTH);
    printField("passWord", passWord, COL1_WIDTH, COL2_WIDTH);
    printField("response", response, COL1_WIDTH, COL2_WIDTH);
    printField("source", source, COL1_WIDTH, COL2_WIDTH);
    printField("clientType", numberToString(clientType), COL1_WIDTH, COL2_WIDTH);
    
    std::cout << formatServerMessage("DEBUG", "+-----------------+-------------------------------+") << std::endl;
}
