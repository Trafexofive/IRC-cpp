/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 21:54:40 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/17 09:54:01 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "server.hpp"
class _client
{
    private :
        int fd_client;
        bool auth;
        bool connected;
        std::string ip_addr;
        std::string fullname;
        std::string nickname;
        std::string password;
        std::string buff;
        std::string response;
        // bool    completed_cmd;
        // std::vector<std::vector <std::string>> parsed;
        struct sockaddr_in client_infos;
    public :
    void    authenticate();
    _client()
    {}
    bool get_bool()
    {
        return auth;
    }
    bool get_connect()
    {
        return connected;
    }
    void    set_connect(bool i)
    {
        connected = i;
    }
    void    set_bool(bool i)
    {
        auth = i;
    }
    void    set_response(std::string _resp)
    {
        response = _resp;
    }
    std::string    &get_response()
    {
        return response;
    }
    _client (int fd, struct sockaddr_in ddr) : fd_client(fd),auth(false),client_infos(ddr){}
        int get_fd()
        {
            return fd_client;
        }
        void    set_fd(int fd)
        {
            fd_client = fd;
        }
        std::string get_ip()
        {
            return ip_addr;
        }
        void    set_ip(std::string ip)
        {
            ip_addr = ip;
        }
        void    set_user(std::string ip)
        {
            fullname = ip;
        }
        std::string    get_user()
        {
            return fullname;
        }
        void    set_nick(std::string ip)
        {
            nickname = ip;
        }
        std::string    get_nick()
        {
            return nickname;
        }
        void    set_pass(std::string ip)
        {
            password = ip;
        }
        std::string    get_pass()
        {
            return password;
        }
        std::string get_buff()
        {
            return buff;
        }
        void    set_buff(std::string _buff,int flag)
        {
            if (flag)
            {
                buff += _buff;
            }
            else
                buff = _buff;
        }
        struct sockaddr_in &get_info()
        {
            return client_infos;
        }
        void    set_info(struct sockaddr_in info)
        {
            client_infos = info;
        }
};

class channel

{
    private:
        std::string name;
        std::string topic;
        std::string password;
        std::vector<_client> members;
    public :
    std::string getName() { return name; }
    void setName(const std::string& n) { name = n; }

    std::string getTopic() { return topic; }
    void setTopic(const std::string& t) { topic = t; }

    std::string getPassword() { return password; }
    void setPassword(const std::string& p) { password = p; }

    const std::vector<_client>& getMembers() const { return members; } 
    void addMember(_client member) { members.push_back(member); } 

};

#endif