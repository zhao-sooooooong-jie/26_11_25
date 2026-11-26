#pragma once

#include<iostream>
#include<sys/select.h>
#include<string>
#include<cstdint>
#include"logger.h"
#include"Socket.hpp"

using namespace NS_SOCKET_MODULE;

static const int gfdnum = sizeof(fd_set) * 8;
static const int gdefaultfd = -1;

class SelectServer{
public:
    SelectServer(uint16_t port = 8080)
    :_port(port),
     _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildTcpSocketMethod(port);
        LOG_DEBUG("create listensock success");

        for(int i = 0; i < gfdnum; i++)
        {
            _refset[i] = gdefaultfd;
        }
        _refset[0] = _listensock->Sockfd();
    }

    void HandlerEvent(fd_set rfds)
    {
        for(int i = 0; i < gfdnum; i++)
        {
            if(_refset[i] == gdefaultfd)
            {
                continue;
            }
            else
            {
                if(FD_ISSET(_refset[i], &rfds))
                {
                    if(_refset[i] == _listensock->Sockfd())
                    {
                        LOG_DEBUG("listensock ready!!!");
                        InetAddr clientsock;
                        int sockfd = _listensock->Accepter(clientsock);
                        int pos = 1;
                        for(; pos < gfdnum; pos++)
                        {
                            if(_refset[pos] == gdefaultfd)
                            {
                                _refset[pos] = sockfd;
                                break;
                            }
                        }
                        if(sockfd == gfdnum)
                        {
                            LOG_WARN("_refset is full");
                            close(sockfd);
                        }
                    }
                    else
                    {
                        char buffer[1024];
                        ssize_t n = recv(_refset[i], buffer, sizeof(buffer) - 1, 0);
                        if(n > 0)
                        {
                            buffer[n] = 0;
                            LOG_DEBUG("buffer:" << buffer);
                            std::string echo = "echo#";
                            echo += buffer;
                            send(_refset[i], buffer, sizeof(buffer), 0);
                        }
                        else if(n == 0)
                        {
                            close(_refset[i]);
                            _refset[i] = gdefaultfd;
                            LOG_WARN("client quit");
                            break;//出循环但是还在流里面，下次循环依旧可以读
                        }
                        else{
                           close(_refset[i]);
                            _refset[i] = gdefaultfd;
                            LOG_ERROR("recv error");
                            break; 
                        }
                    }
                }
            }
        }
    }

    void Loop()
    {
        while(true)
        {
            int maxfd = gdefaultfd;
            fd_set rfds;
            FD_ZERO(&rfds);

            for(int i = 0; i < gfdnum; i++)
            {
                if(_refset[i] == gdefaultfd)
                {
                    continue;
                }
                FD_SET(_refset[0], &rfds);
                if(maxfd < _refset[i])
                {
                    maxfd = _refset[i];
                }
            }
            int n = select(maxfd, &rfds, nullptr, nullptr, nullptr);
            switch(n)
            {
                case 0:
                LOG_INFO("timeout");
                break;
                case -1:
                LOG_WARN("select error");
                break;
                default:
                LOG_INFO("event ready:" << n);
                HandlerEvent(rfds);
            }
        }
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;

    int _refset[gfdnum];
};