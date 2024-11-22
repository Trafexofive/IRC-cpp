// /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mboutuil <mboutuil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 05:27:29 by mboutuil          #+#    #+#             */
/*   Updated: 2024/11/22 14:39:01 by mboutuil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/server.hpp"

void    Core_Server::create_socket()
{
    _socket = socket(AF_INET,SOCK_STREAM,0);
    if (_socket < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit (1);
    }
    int flag = fcntl(_socket,F_GETFL,0);
    fcntl(_socket,F_SETFL,flag | O_NONBLOCK);
    int optval = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(port);
    _server_addr.sin_addr.s_addr = INADDR_ANY;
}

void    Core_Server::bind_sock()
{
    if (bind(_socket ,(struct sockaddr *)&_server_addr,sizeof(_server_addr))< 0)
    {
        std::cout << "error biding the socket" << std::endl;
        exit (1);
    }
}
void    Core_Server::start_listening()
{
    if (listen(_socket,5) < 0)
    {
        std::cout << "error" << std::endl;
        exit (1);
    }
    std::cout << "start listening on port:"<< port << "..." << std::endl;
    // std::cout << "here";
}
void Core_Server::handle_clients()
{
    int kq;
    struct kevent kev;
    int client_fd;

    std::cout << "here";
    kq = kqueue();
    EV_SET(&kev,_socket,EVFILT_READ,EV_ADD | EV_ENABLE,0,0,0);
    kevent(kq,&kev,1,NULL,0,NULL);
    while (1)
    {
        struct kevent *event;
        int nevent = kevent(kq,NULL,0,event,1,NULL);
        for(int i = 0;i < nevent;i++)
        {
            if (event[i].filter == EVFILT_READ)
            {
                client_fd = accept(_socket,NULL,NULL);
                std::cout << "client client fd accepted" << std::endl;
            }
        }
    }
}

void    Core_Server::start_server()
{
    create_socket();
    bind_sock();
    start_listening();
    // std::cout << "here" ;
    int kq = kqueue();
    if (kq < 0)
    {
        std::cout << "error";
        exit (1);
    }
    // int client_fd;
    // std::cout << "here";
    struct kevent ev;
    EV_SET(&ev,_socket,EVFILT_READ , EV_ADD | EV_ENABLE,0,0,NULL);
    if (kevent(kq,&ev,1,NULL,0,NULL) < 0)
    {
        std::cout << "error"<<std::endl;
        exit (1);
    }
    int fd_c;
    struct timespec *timeout = NULL;
    // struct kevent evv;
    struct kevent events[1000];
    while (1)
    {
        // std::cout << "here";
        int nevents = kevent(kq,NULL,0,events,1000,NULL);
        if (nevents < 0)
        {
            std::cout << "error"<<std::endl;
            break ;
        }
        else if (nevents == 0)
        {
            std::cout << "no clients yet" << std::endl;
            continue;
        }
        else if (events != nullptr && nevents > 0)
        {
            // std::cout << "entered";
             for (int i = 0;i< nevents;i++)
             {
                 if (events[i].filter == EVFILT_READ)
                 {
                    if (events[i].ident == _socket)
                    {
                        fd_c = accept(_socket,NULL,NULL);
                         if (fd_c < 0)
                            std::cout << "failure connencting client" << std::endl;
                        else
                        {
                            std::cout << "client accepted FD:" << fd_c << std::endl;
                            // exit (0);
                        }
                        EV_SET(&ev,_socket,EVFILT_READ , EV_ADD | EV_ENABLE,0,0,NULL);
                        if (kevent(kq,&ev,1,NULL,0,NULL) < 0)
                        {
                            std::cout << "error"<<std::endl;
                            exit (1);
                        }
                        struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    getpeername(fd_c, (struct sockaddr*)&client_addr, &client_len);

                    // Print client IP address and port
                    char host[NI_MAXHOST];
                    char service[NI_MAXSERV];
                    getnameinfo((struct sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
                    std::cout << "Client IP: " << host << ", Service: " << service << std::endl;
                    }
                    else
                    {
                        char buff[1024];
                        std::cout << "entered";
                        size_t readed = read(events[i].ident,buff,1024);
                        if (readed <= 0)
                        {
                            std::cout << "closing fd connection" << events[i].ident << std::endl;
                            close (events[i].ident);
                        }
                        else
                        {
                            std::cout << "data received" << std::string (buff,readed) << std::endl;
                            // write (events[i].ident,buff,readed);
                        }
                    }
                    
                }
            }
                    }
                    else
                    {
                        std::cout << "error keeping server" << std::endl;
                        break ;
                    }
                    // usleep(10000);
                }
                // std::cout << fd_c << std::endl;
    // EV_SET(&events[0],_socket,EVFILT_WRITE , EV_ADD | EV_CLEAR,0,0,NULL);
    // if (events[0].filter == EVFILT_READ)
    // {
    //     std::cout << "client accepted" << std::endl;
    // }

    close (kq);
}