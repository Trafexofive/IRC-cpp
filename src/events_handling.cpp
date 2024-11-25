/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events_handling.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 22:41:53 by mboutuil          #+#    #+#             */
/*   Updated: 2024/11/24 01:46:14 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/server.hpp"

std::string Core_Server::send_welcome_message(_client& cl) {
    std::ostringstream response;

    // 001 RPL_WELCOME
    response << "001 " << cl.get_nick() << " :Welcome to the Internet Relay Network "
             << cl.get_nick() << "!" << cl.get_user() << "@" << cl.get_ip() << "\r\n";

    // 002 RPL_YOURHOST
    response << "002 " << cl.get_nick() << " :Your host is irc.example.com, running version 1.0\r\n";

    // 003 RPL_CREATED
    response << "003 " << cl.get_nick() << " :This server was created Mon Nov 20 2024\r\n";

    // 004 RPL_MYINFO
    response << "004 " << cl.get_nick() << " irc.example.com 1.0 iw opsitn\r\n";

    return response.str();
}

void    Core_Server::process_command(_client &client,std::string message)
{
    bool flag =false;
    std::vector<std::string> full_mess;
    std::vector<std::string>::iterator it;

    int o = message.find_first_of("\r\n");
    if (!o)
    {
        client.set_buff(message,1);
        return ;
    }
    std::istringstream iss(message.substr(0,o));
    it = full_mess.begin();
    std::string _buff;
    while (std::getline(iss,_buff,' '))
    {
        if (!_buff.empty())
            full_mess.push_back(_buff);
    }
    std::string command = full_mess[0];
    full_mess.erase(full_mess.begin());
    if (!client.get_connect() && command != "PASS")
    {
        client.set_response("462 ERR_ALREADYREGISTRED :You must authenticate with PASS before sending other commands\r\n");
        return ;
    }
    else if (!client.get_bool())
    {
        if (command == "PASS" && !client.get_connect())
        {
            if (passwd == full_mess[0])
            {
                client.set_connect(true);
                client.set_response("you have been connected !\r\n");
                return ;
            }
            else
            {
                // std::cout <<full_mess[0] << "-"<
                client.set_response("464 ERR_PASSWDMISMATCH :Password incorrect\r\n");
                return ;
            }
        }
        else if (command != "NICK" && command != "USER" && client.get_connect())
        {
                client.set_response("451 ERR_NOTREGISTERED :You must set your nickname with NICK before proceeding.\r\n");
                return ;
        }
        if (command == "NICK")
        {
            client.set_nick(full_mess[0]);
            // return ;
        }
        else if (command == "USER")
        {
            client.set_user(full_mess[0]);
            // return ;
        }
        if (client.get_connect() && !client.get_nick().empty() && !client.get_user().empty())
         {
            // client.set_nick(full_mess[0]);
            client.set_bool(true);
            client.set_response(send_welcome_message(client));
            return ;
         }
        else if (client.get_user().empty() && client.get_nick().empty())
        {
            client.set_response("451 ERR_NOTREGISTERED :You must set your nickname with NICK before proceeding.\r\n");
            // client.set_bool(false);
            return ;
        }
    }
}