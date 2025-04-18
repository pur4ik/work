#include "netcat.h"

Netcat commander;

void Netcat::command() {
        std::string command = "nc " + ip + " " + std::to_string(tcp_port) + " < " + request_file + " > " + response_file;
        if (system(command.c_str()) != 0) {
            std::cerr << "Ошибка при выполнении команды netcat." << std::endl;
        }

}

void Netcat::exit() {
    std::string exit_command = "pkill nc";
    if (system(exit_command.c_str()) != 0) {
        std::cerr << "Ошибка при завершении netcat." << std::endl;
    }
}