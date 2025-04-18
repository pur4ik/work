#include "utils.h"
#include <iostream>
Utils util;

std::string Utils::convert_float(float value, char mode) {
    value *= 10;
    std::string command;
    if (mode == 'u') {
        command += std::to_string(static_cast<int>(value));
        while (command.length() < 5) {
            command = "0" + command;
        }
        command = "su" + command;
    } else {
        if (mode == 'i') {
            command += std::to_string(static_cast<int>(value));
            while (command.length() < 5) {
                command = "0" + command;
            }
            command = "si" + command;
        }
    }
    command += "\r\n";
    return command;
}

float Utils::convert_string(std::string value2) {
    float converted;
    size_t pos_v = value2.find("rv");
    size_t pos_i = value2.find("ri");
    if (pos_v != std::string::npos) {
        value2.erase (pos_v, 2);
        converted = std::stof(value2) / 10;
    } else {
        if (pos_i != std::string::npos) {
            value2.erase (pos_i, 2);
             converted = std::stof(value2) / 10;
        }   
    }
    return converted;
}