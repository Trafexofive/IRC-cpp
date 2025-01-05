/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 19:37:04 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/24 19:37:04 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"

// ################################## CONNECTIONS

bool Bot::connectToServer() {

    _env.setClientFdSocket(socket(AF_INET, SOCK_STREAM, 0));
    if (_env.getClientFdSocket() == -1) {
        std::cerr << "Error: Can't create socket" << std::endl;
        return false;
    } else {
        std::cout << "Info: Socket created" << std::endl;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(_env.getPort());
    server.sin_addr.s_addr = inet_addr(_env.getServerAddress().c_str());

    if (connect(_env.getClientFdSocket(), (struct sockaddr *)&server,
                sizeof(server)) < 0) {
        std::cerr << "Error: Connection failed" << std::endl;
        return false;
    } else {
        std::cout << "Info: Connected to server" << std::endl;
    }
    
    return true;

}

void Bot::disconnectFromServer() {
    close(_env.getClientFdSocket());
    if (_env.getClientFdSocket() == -1) {
        std::cerr << "Error: Can't close socket" << std::endl;
    } else {
        std::cout << "Info: Disconnected from server" << std::endl;
    }
}

bool    Bot::registerBot() {
  sendMessageToServer("PASS " + _env.getPassword() + "\r\n");
  sendMessageToServer("NICK " + _env.getNickname() + "\r\n");
  sendMessageToServer("USER " + _env.getUsername() + " 0 * :" + _env.getBotName() +
                      " IRC Bot\r\n");
  // if (!processServerResponse())
  //   return false;
  if (_env.isAutoJoinChannel()) {
    joinChannel();
  }
  return true;
}

