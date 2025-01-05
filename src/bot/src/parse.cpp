/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 19:35:34 by mlamkadm          #+#    #+#             */
/*   Updated: 2024/09/24 19:35:34 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Parse.hpp"
#include "../inc/Bot.hpp"

void Bot::parseArgs(int argc, char **argv) {
  if (argc <= 3) {
    std::string arg = argv[1];
    if (arg == "-h" || arg == "--help") {
      std::cout << "tldr : ./bot start" << std::endl;
      std::cout << "Usage: bot [OPTION...]" << std::endl;
      std::cout
          << " -C, --config-file=FILE     use this user configuration file"
          << std::endl;
      std::cout << " -d, --debug                 emit debugging messages"
                << std::endl;
      std::cout << " -D, --default               reset all options to their "
                   "default values"
                << std::endl;
      std::cout
          << " -v, --version               output version information and "
             "exit";
      std::cout << " -h, --help                  display this help and exit"
                << std::endl;

      exit(0);
    } else if (arg == "-v" || arg == "--version") {
      std::cout << "Bot version 1.1" << std::endl;
      exit(0);
    } else if (arg == "-d" || arg == "--debug") {
      _debug = true;
    } else if (arg == "-D" || arg == "--default") {
      _env = Env();
    } else if (arg == "-i" || arg == "--interactive") {
      std::cout << "Interactive mode" << std::endl;
    } else if (arg == "-C" || arg == "--config-file") {
      this->parseConfigFile(argv[2]);
    } else {
      std::cerr << "Unknown option" << std::endl;
      exit(1);
    }
  }
}

std::string resolveIP(const std::string &hostname) {
  std::string ip;
  struct hostent *he;

  // will only resolve the local host
  if ((he = gethostbyname(hostname.c_str())) == NULL) {
    return ip;
  }
  return ip;
}

void Bot::handleServerAddr(const std::string &line) {
  _env.setDomainName(line.substr(line.find("=") + 2));
  _env.setServerAddress(resolveIP(_env.getDomainName()));
}

void Bot::parseConfigFile(const std::string &filename) {
  std::ifstream file(filename);
  std::string line;
  int index = 0;
  while (std::getline(file, line)) {
    if (line[0] == '#' || line.empty())
      continue;
    else if (!handleConfigLine(line)) {
      std::cerr << "Warning: Can't Parse line at Index :" << index << std::endl;
    } else {
      std::cerr << "Warning: Unknown configuration option at line: " << line
                << std::endl;
    }
    index++;
  }
}

bool Bot::loadChannels(const std::string &line) {
  std::string channel = line.substr(line.find("=") + 2);
  _env.addChannel(channel);
  return true;
}

bool Bot::handleConfigLine(const std::string &line) {
  // switch case for each line

  if (line.find("server") != std::string::npos) {
    handleServerAddr(line);
  } else if (line.find("port") != std::string::npos) {
    _env.setPort(std::stoi(line.substr(line.find("=") + 2)));
  } else if (line.find("botname") != std::string::npos) {
    _env.setBotName(line.substr(line.find("=") + 2));
  } else if (line.find("nickname") != std::string::npos) {
    _env.setNickname(line.substr(line.find("=") + 2));
  } else if (line.find("password") != std::string::npos) {
    _env.setPassword(line.substr(line.find("=") + 2));
  } else if (line.find("channel") != std::string::npos) {
    _env.setChannelName(line.substr(line.find("=") + 2));
  } else if (line.find("autojoin") != std::string::npos) {
    _env.setAutoJoinChannel(true);
  } else if (line.find("debug") != std::string::npos) {
    _env.setDebug(true);
  } else if (line.find("runtime") != std::string::npos) {
    _env.setUseRuntime(true);
  } else if (line.find("master") != std::string::npos) {
    return loadMasters(line);
  } else if (line.find("channel") != std::string::npos) {
    return loadChannels(line);
  } else {
    return false;
  }
  return true;
}

bool Bot::isMaster(const std::string &user) {
    for (auto it = _env.getChannels().begin(); it != _env.getChannels().end();
         it++) {
        if (user == *it)
        return true;
    }
  return false;
}

bool Bot::isUser(const std::string &user) {
  for (auto it = _env.getChannels().begin(); it != _env.getChannels().end();
       it++) {
    if (user == *it)
      return true;
  }
  return false;
}

bool Bot::isChannel(const std::string &channel) {
  if (channel[0] == '#')
    return true;
  return false;
}

bool Bot::isCommand(const std::string &line) {
  if (line.find("!cmd") != std::string::npos)
    return true;
  return false;
}

std::string Bot::getCommand(const std::string &response) {
  std::string command = response.substr(response.find("!cmd") + 5);
  command = command.substr(0, command.find(" "));
  // command = command.substr(0, command.size() - 1);
  return command;
}

std::string Bot::getCommandArgs(const std::string &response) {
  // find /cmd then add first word after /cmd to vector

  std::string args;

  if (response.find("!cmd") != std::string::npos) {
    args = response.substr(response.find("!cmd") + 5);
  }
  return args;
}


bool Bot::minimumAllowedArgs() {
  if (_env.getServerAddress().empty() || _env.getDomainName().empty() ||
      _env.getPort() == 0 || _env.getBotName().empty() ||
      _env.getNickname().empty() || _env.getPassword().empty() ||
      _env.getChannelName().empty()) {
    return false;
  }
  return true;
}
