/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 22:05:32 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/12/07 22:05:32 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/core_server.hpp"

int main(int ac, char **av) {
  if (ac != 3 && (!av[1] || !av[2])) {
    std::cout << "ENTER VALID PARAMETERS !!" << std::endl;
    return 1;

  } else {
    std::string port(av[1]);
    std::string passwd(av[2]);

    CoreServer IrcServ(port, passwd);
  }
}
