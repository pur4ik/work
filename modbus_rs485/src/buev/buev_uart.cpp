#include "buev_uart.h"

#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>

Buev_uart uart("/dev/ttyUSB0", 115200);

void Buev_uart::updateTimer() {
    auto now = std::chrono::steady_clock::now();  // Текущее время
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime).count();

    if (elapsed >= 1) {        // Если прошла 1 секунда
        time = true;           // Устанавливаем time в true
        lastUpdateTime = now;  // Обновляем время последнего обновления
    }
};

void Buev_uart::start_parse(const std::string &str) {
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
        std::string temp_value = std::regex_replace(str, std::regex("[^0-9]"), "");
        // std::cout << "temp= " << temp << std::endl;
        data.temp_value = std::stoi(temp_value);
    } else {
        // std::cout << "finished start" << "\n";
        parse(str, seven, nine, zero, test);
    }
}

void Buev_uart::separator(std::string &inconstant_string, int counter) {
    try {
        if (counter == 13) {
            // std::cout << "1st: " << constant_string << "\n" << inconstant_string << std::endl;
        }
        // std::cout << "started separator" << "\n";
        int count = 0;
        for (size_t i = 0; i < inconstant_string.size(); ++i) {
            // std::cout << "sep1" << "\n";
            if (inconstant_string[i] == ',') {
                // std::cout << "sep2" << "\n";
                ++count;
                if (count == counter) {
                    // std::cout << "sep3" << "\n";
                    pos = i;
                    // std::cout << "sep4" << "\n";
                    break;
                }
            }
        }

        constant_string = inconstant_string.substr(0, pos);
        // std::cout << "sep5 " << pos << "\n";
        inconstant_string = inconstant_string.substr(pos, std::string::npos);
    } catch (const std::out_of_range &e) {
        std::cerr << "Ошибка1: " << e.what() << std::endl;
    }
    // std::cout << "sep6" << "\n";

    // if (counter == 9){
    //   std::cout << "1st: "<< constant_string << "\n" << inconstant_string << std::endl;
    // }

    // if (counter == 20){
    // std::cout << "1st: "<< constant_string << "\n" << inconstant_string << std::endl;
    // }
}

void Buev_uart::finish_parse(std::string &constant_string, int mode) {
    // std::cout << "started finish" << "\n";
    constant_string = std::regex_replace(constant_string, std::regex("[^0-9+-]"), ",");  // замена всех нечисловых символов на запятые
    // std::cout << "1" << "\n";
    constant_string = std::regex_replace(constant_string, std::regex(",+"), ",");  // убираем последовательности запятых
    // std::cout << "2" << "\n";
    if (!constant_string.empty() && constant_string.front() == ',') {
        // std::cout << "3" << "\n";
        constant_string = constant_string.substr(1);  // Убираем запятую в начале строки
        // std::cout << "4" << "\n";
    }
    if (!constant_string.empty() && constant_string.back() == ',') {
        // std::cout << "5" << "\n";
        constant_string = constant_string.substr(0, constant_string.size() - 1);  // Убираем запятую в конце
        // std::cout << "6" << "\n";
    }
    // std::cout << "7" << "\n";
    numbers.clear();
    // std::cout << "8" << "\n";

    std::stringstream ss(constant_string);
    // std::cout << "9" << "\n";
    //  Читаем числа из строки помещаем в массив
    while (std::getline(ss, number, ',')) {
        // std::cout << "10" << "\n";
        numbers.push_back(std::stoi(number));
        // std::cout << "11" << "\n";
    }
    updateTimer();
    if (time) {
        switch (mode) {
            case 1:
                // std::cout << "13" << "\n";
                data.time = numbers[0];
                // std::cout << "1" << "\n";
                data.fwv = numbers[1];
                // std::cout << "2" << "\n";
                data.rsv = numbers[2];
                // std::cout << "3" << "\n";
                data.spf = numbers[3];
                // std::cout << "4" << "\n";
                data.iov = numbers[4];
                // std::cout << "5" << "\n";
                data.ur = numbers[5];
                // std::cout << "6" << "\n";
                data.up = numbers[6];
                // std::cout << "7" << "\n";
                data.uo = numbers[7];
                // std::cout << "8" << "\n";
                data.cp = numbers[8];
                // std::cout << "9" << "\n";
                data.u24 = numbers[10];
                // std::cout << "10" << "\n"; // так задумано для обхода числового названия
                //  параметра "U24"
                data.f = numbers[11];
                // std::cout << "11" << "\n";
                data.r = numbers[12];
                // std::cout << "12" << "\n";
                data.pm = numbers[13];
                // std::cout << "13" << "\n";

                break;

            case 7:
                // std::cout << "14" << "\n";
                data.zp = numbers[0];      // нулевая точка АЦП тока (26700 - флешка не прочитана)
                data.corph1 = numbers[1];  // корректировка фаз
                data.corph2 = numbers[2];
                data.corph3 = numbers[3];
                data.pha = numbers[4];  //
                data.phb = numbers[5];  //
                data.phc = numbers[6];

                break;

            case 0:
                // std::cout << "15" << "\n";
                if (test) {
                    // std::cout << "16" << "\n";
                    data.pt = numbers[0];
                    data.pr = numbers[1];
                    data.div = numbers[2];
                    data.spi = numbers[3];
                    data.test = numbers[4];
                } else {
                    // std::cout << "17" << "\n";
                    data.pt = numbers[0];
                    data.pr = numbers[1];
                    data.div = numbers[2];
                    data.spi = numbers[3];
                    data.wm = std::to_string(numbers[4]);
                    data.test = numbers[5];
                }

                break;

            case 9:
                // std::cout << "18" << "\n";
                data.ig = numbers[0];
                data.iab = numbers[1];
                data.uab = numbers[2];
                data.fw = numbers[3];

                break;

            default:
                break;
        }
        time = false;
    }
}

Buev_uart::ParsedData Buev_uart::parse(const std::string &str, bool seven, bool nine, bool zero, bool test) {
    inconstant_string = std::regex_replace(str, std::regex("[^a-zA-Z0-9+-]"), ",");  // убираем все символы кроме цифр и +/-

    // Убираем последовательности запятых и удаляем ведущие и завершающие
    // запятые
    inconstant_string = std::regex_replace(inconstant_string, std::regex(",+"), ",");
    if (!inconstant_string.empty() && inconstant_string.front() == ',') {
        inconstant_string = inconstant_string.substr(1);  // Убираем запятую в начале строки
    }

    // std::cout << "result" << inconstant_string << std::endl;

    if (inconstant_string.back() != ',') {
        inconstant_string += ",";
    }

    separator(inconstant_string, 20);
    finish_parse(constant_string, 1);

    // std::cout << "time: " << data.time << std::endl;
    //  std::cout << "fwv: " << data.fwv << std::endl;
    //  std::cout << "rsv: " << data.rsv << std::endl;
    //  std::cout << "spf: " << data.spf << std::endl;
    //  std::cout << "iov: " << data.iov << std::endl;
    //   std::cout << "ur: " << data.ur << std::endl;
    //   std::cout << "up: " << data.up << std::endl;
    //   std::cout << "uo: " << data.uo << std::endl;
    //   std::cout << "cp: " << data.cp << std::endl;
    //  std::cout << "u24: " << data.u24 << std::endl;
    //    std::cout << "f: " << data.f << std::endl;
    //    std::cout << "r: " << data.r << std::endl;
    //   std::cout << "pm: " << data.pm << std::endl;

    if (seven) {
        // 13 зпт
        separator(inconstant_string, 13);

        finish_parse(constant_string, 7);
    }

    if (zero) {
        separator(inconstant_string, 9);
        if (test) {
            data.wm = "a0";
            std::string rem = ",a0";
            size_t rm = constant_string.find(rem);
            // std::cout << "1" << std::endl;
            // std::cout << rm << " " << rem.length() << std::endl;
            // std::cout << "parse" << "\n";
            constant_string.erase(rm, rem.length());
            // std::cout << "2" << std::endl;
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

int Buev_uart::write_data(const std::string &data) {
    if (write(fd, data.c_str(), data.size()) == -1) {
        return -1;
    } else {
        return 0;
    }
}

// Метод для чтения данных
std::string Buev_uart::read_data(size_t size) {
    char buffer[1000];        // Буфер для чтения
    std::string data_buffer;  // Буфер для накопления строк

    try {
        while (true) {
            // Чтение из порта
            ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';  // Завершаем строку
                data_buffer += buffer;      // Накопление данных

                // Проверка на наличие символа начала строки '['
                size_t start_pos = data_buffer.find_first_of("[TEMPSTART]");
                if (start_pos != std::string::npos) {
                    // Если найден один из символов '[', 'T', 'S', начинаем накопление с первого подходящего символа
                    // Ищем правильное начало строки
                    if (data_buffer[start_pos] == '[' || data_buffer.substr(start_pos, 4) == "TEMP" || data_buffer.substr(start_pos, 5) == "START") {
                        data_buffer.erase(0, start_pos);  // Оставляем данные начиная с найденного символа
                    } else {
                        // Если символы в начале не подходят, очищаем буфер
                        data_buffer.clear();
                        continue;
                    }
                } else {
                    // Если нет ни '[' ни 'TEMP' ни 'START', очищаем буфер
                    data_buffer.clear();
                    continue;
                }

                // Проверка на наличие символа конца строки '\n'
                size_t pos;
                while ((pos = data_buffer.find('\n')) != std::string::npos) {
                    std::string complete_line = data_buffer.substr(0, pos);  // Завершённая строка

                    // Проверка на начало строки: '[', 'TEMP' или 'START'
                    if (complete_line.substr(0, 1) == "[" || complete_line.substr(0, 4) == "TEMP" || complete_line.substr(0, 5) == "START") {
                        buev_log = complete_line;       // Сохраняем строку
                        data_buffer.erase(0, pos + 1);  // Удаляем обработанную строку
                        return buev_log;
                    }

                    // Если строка не начинается с требуемого префикса, удаляем её и продолжаем
                    data_buffer.erase(0, pos + 1);
                }
            }
        }
    } catch (const std::out_of_range &e) {
        std::cerr << "Ошибка2: " << e.what() << std::endl;
        data_buffer.clear();  // Безопасно очищаем буфер в случае ошибки
    }

    return "";  // Возвращаем пустую строку в случае ошибки
}
