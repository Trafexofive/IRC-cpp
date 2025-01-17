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



//loooooooool this is shit. needs some 10xing.
int main(int ac, char** av)
{
    if (ac != 3 || !av[1] || !av[2])
    {
        std::cerr << formatServerMessage("ERROR", "Usage: ./irc-server <port> <password>") << std::endl;
        return 1;
    }
    else 
    {
        // std::cout << formatServerMessage("DEBUG", "Insta")
        CoreServer IrcServ(av[1], av[2]);
    }
    
    return 0;
}
