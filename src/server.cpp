/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 05:18:50 by mboutuil          #+#    #+#             */
/*   Updated: 2024/12/01 14:31:41 by mboutuil         ###   ########.fr       */
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
    if (listen(ServData._socket,5) < 0)
    {
        std::cout << "error" << std::endl;
        exit (1);
    }
    std::cout << "start listening on port:"<< ServData.Port << "..." << std::endl;
    if (bind(ServData._socket ,(struct sockaddr *)&ServData.ServAddr,sizeof(ServData.ServAddr))< 0)
    {
        std::cout << "error biding the socket" << std::endl;
        exit (1);
    }
}

void    CoreServer::start_server()
{
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
            if (fds->revents & POLLIN)
            {
                if (fds[i].fd == ServData._socket)
                {
                    WelcomeClient();
                }
                else
                {
                    WriteEvent(fds[i].fd);
                }
            }
            else
            {

               ReadEvent(fds[i].fd);
            }
        }
    }
}

void    CoreServer::WelcomeClient()
{
    int fd_c;
    fd_c = accept(ServData._socket,NULL,NULL);
      if (fd_c < 0)
        { 
            std::cout << "failure connencting client" << std::endl;
            return ;
        }
     else
        {
             std::cout << "client accepted FD:" << fd_c << std::endl;
        }
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        getpeername(fd_c, (struct sockaddr*)&client_addr, &client_len);
        clients[fd_c] = _client(fd_c,client_addr);

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        getnameinfo((struct sockaddr*)&clients[fd_c].get_info(), sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        clients[fd_c].set_response(":wesuseARCH.com NOTICE * :Please authenticate with PASS, then set your NICK and USER.\r\n");
}

void    CoreServer::ReadEvent(int fd)
{
    char buffer[1024];
    int readed = read(fd,buffer,1024);
    if (readed <= 0)
    {
        std::cout << "closing connection FD:" << fd << std::endl;
        close(fd);
        clients.erase(fd);
        return ;
        // continue;
    }
    else
        printf("%s",buffer);
}

void    CoreServer::WriteEvent(int fd)
{
    size_t k  = 0;
    if (!clients[fd].get_response().empty())
    {
        k = write (fd,clients[fd].get_response().c_str(),clients[fd].get_response().length());
        if (k < 0)
        {
            std::cout << "writing error" << std::endl;
            // return ;
        }
        clients[fd].get_response().erase();
     }
}

bool IsValidPort(std::string port ,int& _tport)
{
    if (!std::all_of(port.begin(),port.end(),::isdigit))
    {
        return false;
    }
    // int _tport;

    try
    {
        _tport = std::stoi(port);
        if (_tport < 1 || _tport > 65535)
            throw false;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
    
}

unsigned long polynomialHash(const std::string &str) {
    const unsigned long p = 31; // Prime number
    const unsigned long m = 1000000007; // Large prime number for modulo operation

    unsigned long hash_value = 0;
    unsigned long p_pow = 1; // p^i
    
    // Loop through each character in the string
    for (size_t i = 0; i < str.length(); ++i) {
        hash_value = (hash_value + (str[i] - 'a' + 1) * p_pow) % m; // Apply the polynomial hash formula
        p_pow = (p_pow * p) % m; // Increase power of p for the next character
    }

    return hash_value;
}

bool   IsValidPass(std::string _pass,unsigned long& passwd)
{
    if (_pass.length() < 8)
        return false;
        bool Lower = false,Upper = false,Digit = false ,Special = false;
        for(int i = 0;i < _pass.length();i++)
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
    passwd = polynomialHash(_pass);
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
}

int main (int ac,char **av)
{
    if (!av[1] || !av[2])
        return 1;
    std::string port(av[1]);
    std::string passwd(av[2]);
        // std::cout << port << std::endl;
        // exit(1);
        CoreServer IrcServ(port,passwd);


}