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
#include <list>


// void signalHandler(int signum)
// {
//     std::cout << formatServerMessage("SYSTEM", "Interrupt signal (" + numberToString(signum) + ") received. Exiting gracefully") << std::endl;
//     exit(signum);
// }


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
    
    return 0;
}
