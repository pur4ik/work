#ifndef SOCKET_H
#define SOCKET_H

#include <net/if.h>
#include <ifaddrs.h>
#include <cstring>      // Для memset
#include <unistd.h>     // Для close
#include <sys/socket.h> // Для сокетов
#include <netinet/in.h> // Для sockaddr_in
#include <arpa/inet.h>
#include <iostream>

class Socket
{
private:
    const char *ip = "192.168.1.209";
    const int port = 19001;
    int socket_fd = -1;

public:
    bool load_connection = false;

    Socket()
    {
        init_socket();
    }

    ~Socket() {close_socket();}

    int listen_reply ();
    void init_socket();
    void close_socket() { close(socket_fd); };
    int send(const char *str);
    bool getState()
    {
        return socket_fd == -1 ? false : true;
    };
};

extern Socket _socket;

#endif