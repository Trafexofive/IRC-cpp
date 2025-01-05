/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 23:44:07 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/26 23:44:07 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Macro.hpp"

class Token {
private:
  std::string _token;
  std::string _type;
};


class Parser {
private:
  std::string _message;
  std::string _command;
  std::vector<std::string> _args;

public:
  std::vector<std::string> getTokens(const std::string &line);




};

#endif
