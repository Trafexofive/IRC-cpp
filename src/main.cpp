/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 05:13:03 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/27 05:13:03 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <cstdlib>
#include <signal.h>


void signalHandler(int signum)
{
    printServerMessage("INFO", "Interrupt signal (" + numberToString(signum) + ") received.");
    exit(signum);
}


int main(int ac, char** av)
{
    if (ac != 3 || !av[1] || !av[2])
    {
        printServerMessage("ERROR", "Usage: ./server <port> <password>");
        return 1;
    }
    else 
    {
        CoreServer IrcServ(av[1], av[2]);
    }
    signal(SIGINT, signalHandler);
    
    return 0;
}
