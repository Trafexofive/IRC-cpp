/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events_handling.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 22:41:53 by mboutuil          #+#    #+#             */
/*   Updated: 2024/11/24 01:35:56 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/server.hpp"

void    Core_Server::process_command(_client &client,std::string message)
{
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
    if (!client.get_bool() && command != "PASS")
    {
        client.set_response("462 ERR_ALREADYREGISTRED :You must authenticate with PASS before sending other commands\r\n");
        return ;
    }
    if (command == "PASS")
    {
        if (passwd == full_mess[0])
        {
            client.set_connect(true);
            client.set_response("you have been connect ! \r\n");
            return ;
        }
        else
        {
            std::cout <<full_mess[0] << "-"<<std::endl;
            std::cout <<passwd << "-"<< std::endl;
            client.set_response("464 ERR_PASSWDMISMATCH :Password incorrect\r\n");
            return ;
        }
    }
}