/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reminder.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 18:43:04 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/25 18:43:04 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Reminder.hpp"
#include "../inc/Bot.hpp"

// canonical form

Reminder::Reminder()
    : _title(""), _message(""), _time(0), _user(""), _channel("") {}

Reminder::Reminder(const Reminder &other) { *this = other; }

Reminder::~Reminder() {}

Reminder &Reminder::operator=(const Reminder &other) {
  _title = other._title;
  _message = other._message;
  _time = other._time;
  _user = other._user;
  _channel = other._channel;
  return *this;
}

Reminder::Reminder(const std::string &title, const std::string &name,
                   const std::string &message, time_t time) {
  if (name[0] == '#') {
    _channel = name;
  } else {
    _user = name;
  }
  _title = title;
  _message = message;
  _time = time;
}

// getters and setters

std::string Reminder::getMessage() const { return _message; }

time_t Reminder::getTime() const { return _time; }

std::string Reminder::getUser() const { return _user; }

std::string Reminder::getChannel() const { return _channel; }

std::string Reminder::getTitle() const { return _title; }

// reminder specific methodes

void Bot::addReminderChannel(const std::string &title,
                             const std::string &message, time_t reminderTime, const std::string &channelName) {
  time_t startTime = getUptime();
  time_t endTime = startTime + reminderTime;
  Reminder reminder(title, channelName, message, endTime);
  _reminders.push_back(reminder);
}

void Bot::addReminderUser(const std::string &title, const std::string &message,
                          time_t reminderTime, const std::string &nickname) {
  time_t startTime = getUptime();
  time_t endTime = startTime + reminderTime;
  Reminder reminder(title, nickname, message, endTime);
  _reminders.push_back(reminder);
}

void Bot::SendReminder(Reminder &reminder) {
  if (reminder.getChannel().empty()) {
    messageUser(reminder.getUser(),
                "User Reminder >--" + reminder.getTitle() + "--<");
    messageChannel("");
    messageUser(reminder.getUser(), "Message > " + reminder.getMessage());
  } else {
    messageChannel(reminder.getChannel(),
                   "Channel Reminder >--" + reminder.getTitle() + "--<");
    messageChannel("");
    messageChannel(reminder.getChannel(), "Message > " + reminder.getMessage());
  }
}

void Bot::checkReminders() {
  time_t currentTime = getUptime();
  for (std::vector<Reminder>::iterator it = _reminders.begin();
       it != _reminders.end(); ++it) {
    if (it->getTime() == currentTime) {
      SendReminder(*it);
    }
  }
}

// bool Bot::loadMasters(const std::string &masters) {
//   std::string master;
//
//
//
//   return true;
// }
