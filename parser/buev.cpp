#include "buev.h"
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>

void Buev::start_parse(const std::string &str) {

    seven = false;
    nine = false;
    zero = false;
    test = false;
    temp = false;

    if (str.find(seven_str) != std::string::npos) {
        seven = true;
    }
    if (str.find(nine_str) != std::string::npos) {
        nine = true;
    }
    if (str.find(zero_str) != std::string::npos) {
        zero = true;
    }
    if (str.find(test_str) != std::string::npos) {
        test = true;
    }
    if (str.find(temp_str) != std::string::npos) {
        temp = true;
    }
    if (str.find(start_str) != std::string::npos) {
        start = true;
    }

    if (start) {
        data.start = true;
        return;
    }

    if (temp) {
        std::string temp = std::regex_replace(str, std::regex("[^0-9]"), "");
        data.temp = std::stoi(temp);
    } else {
        parse(str, seven, nine, zero, test);
    }
}

void Buev::separator(std::string &inconstant_string, int counter) {
    if (counter == 13) {
    }

    int count = 0;
    for (size_t i = 0; i < inconstant_string.size(); ++i) {
        if (inconstant_string[i] == ',') {
            ++count;
            if (count == counter) {
                pos = i;
                break;
            }
        }
    }

    constant_string = inconstant_string.substr(0, pos);
    inconstant_string = inconstant_string.substr(pos, std::string::npos);
}

void Buev::finish_parse(std::string &constant_string, int mode) {
    constant_string = std::regex_replace(constant_string, std::regex("[^0-9+-]"), ",");
    constant_string = std::regex_replace(constant_string, std::regex(",+"), ",");
    if (!constant_string.empty() && constant_string.front() == ',') {
        constant_string = constant_string.substr(1);
    }
    if (!constant_string.empty() && constant_string.back() == ',') {
        constant_string = constant_string.substr(0, constant_string.size() - 1);
    }
    numbers.clear();

    std::stringstream ss(constant_string);
    while (std::getline(ss, number, ',')) {
        numbers.push_back(std::stoi(number));
    }
    switch (mode) {
    case 1:
        data.time = numbers[0];
        data.fwv = numbers[1];
        data.rsv = numbers[2];
        data.spf = numbers[3];
        data.iov = numbers[4];
        data.ur = numbers[5];
        data.up = numbers[6];
        data.uo = numbers[7];
        data.cp = numbers[8];
        data.u24 = numbers[10];
        data.f = numbers[11];
        data.r = numbers[12];
        data.pm = numbers[13];
        break;

    case 7:
        data.zp = numbers[0];
        data.corph1 = numbers[1];
        data.corph2 = numbers[2];
        data.corph3 = numbers[3];
        data.pha = numbers[4];
        data.phb = numbers[5];
        data.phc = numbers[6];

        break;

    case 0:
        if (test) {
            data.pt = numbers[0];
            data.pr = numbers[1];
            data.div = numbers[2];
            data.spi = numbers[3];
            data.test = numbers[4];
        } else {
            data.pt = numbers[0];
            data.pr = numbers[1];
            data.div = numbers[2];
            data.spi = numbers[3];
            data.wm = std::to_string(numbers[4]);
            data.test = numbers[5];
        }

        break;

    case 9:
        data.ig = numbers[0];
        data.iab = numbers[1];
        data.uab = numbers[2];
        data.fw = numbers[3];
        break;

    default:
        break;
    }
}

Buev::ParsedData Buev::parse(const std::string &str, bool seven, bool nine, bool zero, bool test) {

    inconstant_string = std::regex_replace(str, std::regex("[^a-zA-Z0-9+-]"), ","); 
    inconstant_string = std::regex_replace(inconstant_string, std::regex(",+"), ",");
    if (!inconstant_string.empty() && inconstant_string.front() == ',') {
        inconstant_string = inconstant_string.substr(1); 
    }

    if (inconstant_string.back() != ',') {
        inconstant_string += ",";
    }

    separator(inconstant_string, 20);
    finish_parse(constant_string, 1);

    if (seven) {
        separator(inconstant_string, 13);

        finish_parse(constant_string, 7);
    }

    if (zero) {
        separator(inconstant_string, 9);
        if (test) {
            data.wm = "a0";
            std::string rem = ",a0";
            size_t rm = constant_string.find(rem);
            constant_string.erase(rm, rem.length());
        }
        finish_parse(constant_string, 0);
    }

    if (nine) {
        inconstant_string += ",";
        separator(inconstant_string, 9);
        finish_parse(constant_string, 9);
    }
    return data;
}

void Buev::write_data(const std::string &data) {
    if (write(fd, data.c_str(), data.size()) == -1) {
        throw std::runtime_error("Ошибка записи в порт");
    }
}

std::string Buev::read_data(size_t size) {
    char buffer[1000];    
    std::string data_buffer;

    while (true) {
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; 
            data_buffer += buffer;     
            size_t start_pos = data_buffer.find_first_of("[TEMPSTART]");
            if (start_pos != std::string::npos) {
                if (data_buffer[start_pos] == '[' || data_buffer.substr(start_pos, 4) == "TEMP" || data_buffer.substr(start_pos, 5) == "START") {
                    data_buffer.erase(0, start_pos); 
                } else {
                    data_buffer.clear();
                    continue;
                }
            } else {
                data_buffer.clear();
                continue;
            }

            size_t pos;
            while ((pos = data_buffer.find('\n')) != std::string::npos) {
                std::string complete_line = data_buffer.substr(0, pos);
                if (complete_line.substr(0, 1) == "[" || complete_line.substr(0, 4) == "TEMP" || complete_line.substr(0, 5) == "START") {
                    buev_log = complete_line;     
                    data_buffer.erase(0, pos + 1); 
                    return buev_log;
                }

                data_buffer.erase(0, pos + 1);
            }
        }
    }
}
