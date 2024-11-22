/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 05:18:50 by mboutuil          #+#    #+#             */
/*   Updated: 2024/11/22 16:14:07 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/server.hpp"

// void    Core_Server::start_server()
// {
//     _socket = socket(AF_INET,SOCK_STREAM,0);
//     if (_socket < 0)
//     {
//         std::cerr << "socket error"<<std::endl;
//         exit(1);
//     }
//     int flags = fcntl(_socket , F_GETFL,0);
//     fcntl(_socket,F_SETFL,flags | O_NONBLOCK);
// }
int main ()
{
    Core_Server _server("hello",8000);

    _server.start_server();

}