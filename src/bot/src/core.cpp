/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 01:42:05 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/25 01:42:05 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"
#include "../inc/Reminder.hpp"

bool Bot::coreLoop() {

  if (!connectToServer()) {
    return false;
  }
  DisplayBotInfo();
  if (!registerBot()) {
    return false;
  }
  initUptime();

  while (1) {
    if (!processServerResponse())
      return false;
    sleep(1);
  }
  return true;
}
