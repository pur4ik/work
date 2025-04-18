#include "netcat.h"

Netcat commander;

void Netcat::command() {
    std::string command = "nc " + ip + " " + std::to_string(tcp_port) + " < " + request_file + " > " + response_file;
    std::cout <<command << " \n";
    if (system(command.c_str()) != 0) {
        std::cerr << "Ошибка при выполнении команды netcat." << std::endl;
    }
}