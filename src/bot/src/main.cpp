/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 20:52:49 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/19 20:52:49 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"
#include "../inc/Reminder.hpp"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main(int ac, char **av) {

  {
    Bot bot;
    bot.parseArgs(ac, av);
    if (!bot.minimumAllowedArgs())
      return 1;
    bot.coreLoop();
  }

  return 0;
}
