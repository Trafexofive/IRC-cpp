#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "Utils.hpp"

class _client
{
    private:
        int fd_client;
        bool auth;
        bool connected;
        struct sockaddr_in client_infos;
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        std::string buff;
        std::string response;

    public:
        _client();
        _client(int fd, struct sockaddr_in ddr);
        
        void authenticate();
        void clear();
        void clear_response();
        
        // Getters
        int get_fd() const;
        bool get_bool() const;
        bool get_connect() const;
        const std::string& get_ip() const;
        const std::string& get_user() const;
        const std::string& get_nick() const;
        const std::string& get_pass() const;
        const std::string& get_buff() const;
        const std::string& get_response() const;
        const struct sockaddr_in& get_info() const;

        // Setters
        void set_fd(int fd);
        void set_bool(bool i);
        void set_connect(bool i);
        void set_ip(const std::string& ip);
        void set_user(const std::string& user);
        void set_nick(const std::string& nick);
        void set_pass(const std::string& pass);
        void set_response(const std::string& _resp);
        void set_info(const struct sockaddr_in& info);
        void set_buff(const std::string& _buff, bool append = false);
};

class channel
{
    private:
        std::string name;
        std::string topic;
        std::string password;
        std::vector<_client> members;

    public:
        channel();
        channel(const std::string& n);
        ~channel() {}

        // Getters
        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::string& getPassword() const;
        const std::vector<_client>& getMembers() const;
        std::string getMembersList() const;
        
        // Setters
        void setName(const std::string& n);
        void setTopic(const std::string& t);
        void setPassword(const std::string& p);
        
        void addMember(const _client& member);
        bool removeMember(const std::string& nickname);
        void broadcast(const std::string& message, const std::string& except_nick = "");
};

#endif
