/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 05:18:50 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/02 10:05:32 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/core_server.hpp"

void    CoreServer::create_socket()
{
    ServData._socket = socket(AF_INET,SOCK_STREAM,0);
    if (ServData._socket < 0)
    {
        std::cerr << "socket error"<<std::endl;
        exit(1);
    }
    int flags = fcntl(ServData._socket , F_GETFL,0);
    fcntl(ServData._socket,F_SETFL,flags | O_NONBLOCK);
    int optval = 1;
    setsockopt( ServData._socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    ServData.ServAddr.sin_family = AF_INET;
    ServData.ServAddr.sin_port = htons(ServData.Port);
   ServData.ServAddr.sin_addr.s_addr = INADDR_ANY;
}

void    CoreServer::start_listening()
{
    if (bind(ServData._socket ,(struct sockaddr *)&ServData.ServAddr,sizeof(ServData.ServAddr))< 0)
    {
        std::cout << "error biding the socket" << std::endl;
        exit (1);
    }
    if (listen(ServData._socket,5) < 0)
    {
        std::cout << "error" << std::endl;
        exit (1);
    }
    std::cout << "start listening on port:"<< ServData.Port << "..." << std::endl;
}

void    CoreServer::start_server()
{
    bzero(fds,sizeof(fds));
    fds[0].fd = ServData._socket;
    fds[0].events = POLLIN;

    while (1)
    {
        int ret = poll(fds,1024,5000);
        if (ret < 0)
        {
            std::cout << "poll error" << std::endl;
            break ;
        }
        for (int i = 0;i < 1024;i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == ServData._socket)
                {
                    WelcomeClient();
                }
                else
                {
                    ReadEvent(fds[i].fd);
                }
            }
            else if (fds[i].revents & POLLOUT)
            {
                WriteEvent(fds[i].fd);

            }
        }
    }
}


bool IsValidPort(std::string port ,int& _tport)
{
    if (!std::all_of(port.begin(),port.end(),::isdigit))
    {
        return false;
    }
    // int _tport;

    _tport = std::stoi(port);
    if (_tport < 1 || _tport > 65535)
        return false;
    return true;
    
}

bool   IsValidPass(std::string _pass,std::string& passwd)
{
    if (_pass.length() < 8)
        return false;
        bool Lower = false,Upper = false,Digit = false ,Special = false;
        for(size_t i = 0;i < _pass.length();i++)
        {
            if (std::islower(_pass[i])) Lower = true;
            else if (std::isupper(_pass[i])) 
                Upper = true;
            else if (std::isdigit(_pass[i]))
                Digit = true;
            else if (std::ispunct(_pass[i])) 
                Special = true;
        }
        if ((Lower && Upper && Digit && Special)== false)
            return  false;
    passwd = _pass;
    // passwd = polynomialHash(_pass);
    return true;
}


CoreServer::CoreServer(std::string port,std::string password)
{
    if (!IsValidPort(port ,ServData.Port))
    {
        std::cout << "ENTER VALID PORT !!" << std::endl;
        return ;
    }
    if (!IsValidPass(password,ServData.Passwd))
    {
        std::cout << "ENTER VALID PASSWORD !!" << std::endl;
        std::cout << "At least 1 lowercase letter (a-z) !!" << std::endl;
        std::cout << "At least 1 uppercase letter (A-Z) !!" << std::endl;
        std::cout << "At least 1 digit (0-9) !!" << std::endl;
        std::cout << "At least 1 special character (e.g., !, @, #, $, etc.) !!" << std::endl;
        return ;
        // throw "INVALID  PASSWORD";
    }
    create_socket();
    start_listening();
    start_server();
}

int main (int ac,char **av)
{
    if (ac != 3 && (!av[1] || !av[2]))
    {
        std::cout << "ENTER VALID PARAMETERS !!" << std::endl;
        return 1;

    }
    std::string port(av[1]);
    std::string passwd(av[2]);
        // std::cout << port << std::endl;
        // exit(1);
    CoreServer IrcServ(port,passwd);


}