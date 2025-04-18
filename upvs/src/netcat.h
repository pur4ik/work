#ifndef NETCAT_H
#define NETCAT_H

#include <string>
#include <iostream>



class Netcat {
    private:
    const std::string ip = "10.7.8.101";
    int tcp_port = 19001;
    public:
    const std::string request_file = "request.xml";
    const std::string response_file = "response.xml";
    void command();
};
extern Netcat commander;
#endif