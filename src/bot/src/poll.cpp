/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 04:40:00 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/25 04:40:00 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Poll.hpp"

// Poll::Poll(const std::string &question, const std::vector<std::string>
// &answers, const std::vector<int> &votes) {
//     _question = question;
//     _answers = answers;
//     _votes = votes;
// }

Poll::Poll(const Poll &other) {
  _question = other._question;
  _answers = other._answers;
  _votes = other._votes;
}

Poll &Poll::operator=(const Poll &other) {
  _question = other._question;
  _answers = other._answers;
  _votes = other._votes;
  return *this;
}

void setQuestion(const std::string &question) { _question = question; }

void setVotes(const std::vector<int> &votes) { _votes = votes; }

std::string getQuestion() const { return _question; }

std::vector<int> getVotes() const { return _votes; }

void displayPoll() {
  std::cout << _question << std::endl;
  for (size_t i = 0; i < _answers.size(); i++) {
    std::cout << i + 1 << ". " << _answers[i] << " : " << _votes[i]
              << "| percent "
              << (float)_votes[i] /
                     std::accumulate(_votes.begin(), _votes.end(), 0) * 100
              << "%" << std::endl;
  }
}
