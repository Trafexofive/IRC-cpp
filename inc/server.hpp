/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 04:43:58 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/02 10:07:27 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include<unistd.h>
// #include<sys/syslimits.h>
// #include<stdlib.h>
#include<fstream>
#include<string>

#include<map>
#include<vector>
#include <functional>
#include <netdb.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include <iostream>
#include<sstream>
// #include <sys/epoll.h>
#include <fcntl.h>

#include <sys/types.h>
// #include <sys/event.h>
#include <sys/time.h>
#include <poll.h>
#include "client.hpp"
#include <algorithm>
#define MAX_EVENT 1000
/*
Clients must have a unique name maximum 9 characters
the servers must know the client name and the client host and wich server
is addressed to ==> will be considered in the client class
also there is special clients wich are the operators

*/
#define RPL_WELCOME 001
#define RPL_YOURHOST 002
#define RPL_CREATED 003
#define RPL_MYINFO 004
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_TOPIC 331
#define ERR_ERRONEUSNICKNAME 332	
#define ERR_NICKNAMEINUSE 433	
#define ERR_NEEDMOREPARAMS 461
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NOORIGIN 409



#endif
