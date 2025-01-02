# IRC-cpp

An IRC (Internet Relay Chat) server implementation in C++98.

## Project Description

IRC-cpp is a project aimed at creating a robust IRC client and server using C++. The goal is to provide a lightweight and efficient IRC implementation that can be easily extended and customized.

my initial reason for starting this repo (because of school) was because I wanted to self-host my own irc server, as well as getting into some socket programming. 

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Documentation](#documentation)
- [Current Status](#current-status)
- [Contributing](#contributing)
- [License](#license)

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/Trafexofive/IRC-cpp.git
    ```
2. Navigate into the project directory:
    ```sh
    cd IRC-cpp
    ```
3. Build the project using Makefile:
    ```sh
    make
    ```
4. Build and run directly:
    ```sh
    make build
    ```
5. Alternatively you can display help dialog using::
    ```sh
    make help
    ```

## Usage

To start the IRC server:
```sh
./irc_server
```

To connect to the IRC server with the client:
```sh
./irc_client
```

## File Structure

The project is organized as follows:

```
IRC-cpp/
├── src/                # Source code files
├── include/            # Header files
├── scripts/            # Shell scripts for testing
├── Makefile            # Build configuration
├── doc/                # Documentation files
├── test/               # Test cases and related files
├── log/                # Log files
├── backup/             # Backup files
├── .deps/              # Dependency files
├── README.md           # Project documentation
└── LICENSE             # License file
```

## Documentation

The project includes several documentation files to help you understand and use IRC-cpp effectively:

- [Internet Relay Chat Protocol (RFC 1459)](doc/irc-rfc.md): The official specification of the IRC protocol.
- [The New IRC Channel Operator’s Guide](doc/irchelp_main.md): A guide for new IRC channel operators.
- [Modern IRC RFC](doc/modern-rfc.md): A modernized version of the IRC protocol documentation.

## Current Status

The project is still incomplete. Below are some of the recent changes:

- **small ignore changes** - [Commit Link](https://github.com/Trafexofive/IRC-cpp/commit/f9a20bbe6f2c8dc45f3d2d980847a2a4979a617a)
- **test** - [Commit Link](https://github.com/Trafexofive/IRC-cpp/commit/67835fc51c11db9b661dd098a9916bb0a272c275)
- **todo.md changes** - [Commit Link](https://github.com/Trafexofive/IRC-cpp/commit/99c8cb78a7e7bc99d673aa501d86c5827f98b1fe)
- **latest** - [Commit Link](https://github.com/Trafexofive/IRC-cpp/commit/9e27cca39c3ecbd647021b8fe8ab813483146091)
- **small test.sh enhancements** - [Commit Link](https://github.com/Trafexofive/IRC-cpp/commit/1cb2466cc493a93f38386fd1af275a276441e02f)

For more details, check the [commit history](https://github.com/Trafexofive/IRC-cpp/commits/master).

## Contributing

By all means, Im not the best programmer!
Contributions are welcome! Please fork the repository and submit pull requests with your changes.

## License

This project is licensed under the MIT License.
