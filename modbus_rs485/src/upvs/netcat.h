#ifndef NETCAT_H
#define NETCAT_H

#include <string>
#include <iostream>
#include "../buev/buev_udp.h"



class Netcat {
    private:
    const std::string ip = "10.7.8.101";
    int tcp_port = 19001;
    public:
    const std::string request_file = "xml/request.xml";
    const std::string response_file = "xml/response.xml";
    void command();
    void exit ();
};
extern Netcat commander;
#endif