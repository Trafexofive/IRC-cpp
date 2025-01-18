# IRC Server Project

An IRC (Internet Relay Chat) server implemented in C++. This project allows multiple clients to connect, join channels, and communicate with each other in real-time.

---

## Table of Contents

1. [Features](#features)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [Usage](#usage)
5. [Commands](#commands)
6. [Configuration](#configuration)
7. [Directory Structure](#directory-structure)
8. [Contributing](#contributing)
9. [License](#license)

---

## Features

- **Multi-client support**: Handle multiple clients simultaneously using `poll()`.
- **Channel management**: Create, join, and leave channels.
- **Authentication**: Password-based authentication for clients.
- **Command handling**: Supports standard IRC commands like `NICK`, `USER`, `JOIN`, `PART`, `PRIVMSG`, `PING`, `QUIT`, etc.
- **Logging**: Detailed logging for debugging and monitoring server activity.
- **Cross-platform**: Works on Linux and macOS.

---

## Requirements

- **Compiler**: `g++` or `clang++` (C++98 standard)
- **Libraries**: Standard C++ libraries, `libevent` (for networking)
- **Tools**: `make`, `docker` (optional for containerized deployment)

---

## Installation

### 1. Clone the Repository
```bash
git clone https://github.com/your-username/trafexofive-irc-cpp.git
cd trafexofive-irc-cpp
```

### 2. Build the Project
```bash
make
```

### 3. Run the Server
```bash
./bin/irc-server <port> <password>
```
Example:
```bash
./bin/irc-server 6667 Alilepro135!
```

### 4. (Optional) Docker Setup
Build and run the server using Docker:
```bash
docker build -t irc-server .
docker run -p 6667:6667 irc-server
```

---

## Usage

### Starting the Server
Run the server with a valid port and password:
```bash
./bin/irc-server 6667 Alilepro135!
```

### Connecting to the Server
Use an IRC client (e.g., `irssi`, `hexchat`) or the provided Python TUI client:
```bash
python3 ./test/TUI-client.py -p 6667 --password Alilepro135!
```

### Available Commands
See the [Commands](#commands) section for a list of supported IRC commands.

---

## Commands

| Command   | Description                                      | Example                     |
|-----------|--------------------------------------------------|-----------------------------|
| `PASS`    | Authenticate with the server password            | `PASS Alilepro135!`         |
| `NICK`    | Set your nickname                                | `NICK john`                 |
| `USER`    | Set your username and real name                  | `USER john 0 * :John Doe`   |
| `JOIN`    | Join a channel                                   | `JOIN #general`             |
| `PART`    | Leave a channel                                  | `PART #general`             |
| `PRIVMSG` | Send a message to a user or channel              | `PRIVMSG #general :Hello!`  |
| `PING`    | Check server responsiveness                      | `PING server`               |
| `QUIT`    | Disconnect from the server                       | `QUIT :Goodbye!`            |

---

## Configuration

The server can be configured using the `conf/server.yaml` file. Example configuration:
```yaml
server:
  password: "your_password_here"
  port: 6667

database:
  host: "localhost"
  port: 5432
  user: "your_db_user"
  password: "your_db_password"
  name: "your_db_name"

logging:
  level: "info"
  file: "/var/log/app.log"

security:
  secret_key: "your_secret_key"
```

---

## Directory Structure

```
trafexofive-irc-cpp/
├── Makefile
├── dockerfile
├── conf/
│   └── server.yaml
├── doc/
├── inc/
│   ├── Channel.hpp
│   ├── Client.hpp
│   ├── Conf.hpp
│   ├── Helpers.hpp
│   ├── Server.hpp
│   ├── Utils.hpp
│   ├── debug.hpp
│   ├── ircResponses.hpp
│   └── requestMacros.hpp
├── src/
│   ├── Channel.cpp
│   ├── HandleEvents.cpp
│   ├── Validation.cpp
│   ├── main.cpp
│   ├── channel/
│   │   └── utils.cpp
│   ├── client/
│   │   └── client.cpp
│   ├── commands/
│   │   ├── cmd.cpp
│   │   ├── cmdHelpers.cpp
│   │   ├── join.cpp
│   │   ├── mode.cpp
│   │   ├── nick.cpp
│   │   ├── part.cpp
│   │   ├── pass.cpp
│   │   ├── ping.cpp
│   │   ├── quit.cpp
│   │   └── user.cpp
│   ├── server/
│   │   └── server.cpp
│   └── utils/
│       └── Utils.cpp
└── todo/
```

---

## Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature/YourFeature`).
3. Commit your changes (`git commit -m 'Add some feature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Open a pull request.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

- Inspired by the original IRC protocol (RFC 1459).
- Special thanks to the contributors and open-source community.
