/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reminder.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 18:39:57 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/25 18:39:57 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REMINDER_HPP
#define REMINDER_HPP

#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

class Reminder {
private:
  std::string _title;
  std::string _message;
  time_t _time;

  std::string _user;
  std::string _channel;
  // either user or channel
public:
  Reminder();
  Reminder(const Reminder &other);
  Reminder &operator=(const Reminder &other);
  ~Reminder();

  Reminder(const std::string &title, const std::string &name,
           const std::string &message, time_t time);

  // GETTERS & SETTERS

  std::string getMessage() const;
  time_t getTime() const;
  std::string getUser() const;
  std::string getChannel() const;
  std::string getReminder() const;
  std::string getTitle() const;

  // reminder specific methodes
};

#endif
