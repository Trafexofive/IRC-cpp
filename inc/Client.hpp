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
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        std::string buff;
        std::string response;
        struct sockaddr_in client_infos;

    public:
        _client();
        _client(int fd, struct sockaddr_in ddr);
        
        void authenticate();
        void clear();
        
        // Getters
        int get_fd() const { return fd_client; }
        bool get_bool() const { return auth; }
        bool get_connect() const { return connected; }
        std::string get_ip() const { return ip_addr; }
        std::string get_user() const { return fullname; }
        std::string get_nick() const { return nickname; }
        std::string get_pass() const { return password; }
        std::string get_buff() const { return buff; }
        std::string& get_response() { return response; }
        struct sockaddr_in& get_info() { return client_infos; }

        // Setters
        void set_fd(int fd) { fd_client = fd; }
        void set_bool(bool i) { auth = i; }
        void set_connect(bool i) { connected = i; }
        void set_ip(const std::string& ip) { ip_addr = ip; }
        void set_user(const std::string& user) { fullname = user; }
        void set_nick(const std::string& nick) { nickname = nick; }
        void set_pass(const std::string& pass) { password = pass; }
        void set_response(const std::string& _resp);  // Remove implementation from header
        void set_info(const struct sockaddr_in& info) { client_infos = info; }
        void set_buff(const std::string& _buff, bool append);
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

        // Getters
        std::string getName() const { return name; }
        std::string getTopic() const { return topic; }
        std::string getPassword() const { return password; }
        const std::vector<_client>& getMembers() const { return members; }
        std::string getMembersList() const;  // Add this declaration
        
        // Setters
        void setName(const std::string& n) { name = n; }
        void setTopic(const std::string& t) { topic = t; }
        void setPassword(const std::string& p) { password = p; }
        
        void addMember(const _client& member);
        bool removeMember(const std::string& nickname);
        void broadcast(const std::string& message, const std::string& except_nick = "");  // Add this declaration
};

#endif
