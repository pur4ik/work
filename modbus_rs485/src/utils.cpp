#include "utils.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "freq/freq_converter.h"

Utils util;

void Utils::take_logs(const std::string &log, char type) {
    std::string log2 = log;
    if (type == 'm') {
        log2.insert(0, "[ЧП] ");
    } else {
        if (type == 'b') {
            log2.insert(0, "[BUEV-G]");
        }
        log2.insert(0, "[КН] ");
    }
    _log += log2 + "\n";  // Добавляем новую строку лога
}

int Utils::convert(const std::string &adr) {
    if (adr.size() != 4) {
        throw std::invalid_argument("Input must be a string of 4 characters.");
    }

    // Разбиваем строку на два числа
    std::string part1 = adr.substr(0, 2);
    std::string part2 = adr.substr(2, 2);

    // Переводим каждую часть в десятичное число
    int num1 = std::stoi(part1);
    int num2 = std::stoi(part2);

    // Переводим числа в 16-ричную систему счисления и комбинируем их
    int result = (num1 << 8) | num2;  // Сдвигаем num1 на 8 бит влево и объединяем с num2
                                      //   std::cout << "результат преобразования:" << result<<std::endl;
    return result;                    // segfault
}

std::vector<Utils::Instruction> Utils::parse_scenario(const std::string &scenario) {
    std::vector<Instruction> current_scenario;

    std::map<std::string, std::function<Instruction(const std::vector<std::string> &)>> handlers = {{"f",
                                                                                                     [](const std::vector<std::string> &args) {
                                                                                                         if (args.size() != 2) {
                                                                                                             throw std::invalid_argument("Invalid arguments for freq_change");
                                                                                                         }
                                                                                                         int a = std::stoi(args[0]);
                                                                                                         int b = std::stoi(args[1]);
                                                                                                         return Instruction{[=]() { test2.freq_change(a, b); }};
                                                                                                     }},
                                                                                                    {"s",
                                                                                                     [](const std::vector<std::string> &args) {
                                                                                                         if (args.size() != 1) {
                                                                                                             throw std::invalid_argument("Invalid arguments for sleep");
                                                                                                         }
                                                                                                         int duration = std::stoi(args[0]);
                                                                                                         return Instruction{[=]() { test2.sleep(duration); }};
                                                                                                     }},
                                                                                                    {"start",
                                                                                                     [](const std::vector<std::string> &args) {
                                                                                                         if (!args.empty()) {
                                                                                                             throw std::invalid_argument("Invalid arguments for start");
                                                                                                         }
                                                                                                         return Instruction{[]() {
                                                                                                             if (test2.start() == 0) {
                                                                                                                 // take_logs("Частотный преобразователь запущен!");
                                                                                                             } else {
                                                                                                                 // take_logs("Ошибка запуска частотного преобразователя!");
                                                                                                             }
                                                                                                         }};
                                                                                                     }},
                                                                                                    {"stop", [](const std::vector<std::string> &args) {
                                                                                                         if (!args.empty()) {
                                                                                                             throw std::invalid_argument("Invalid arguments for stop");
                                                                                                         }
                                                                                                         return Instruction{[]() {
                                                                                                             if (test2.stop() == 0) {
                                                                                                                 // take_logs("Частотный преобразователь остановлен!");
                                                                                                             } else {
                                                                                                                 // take_logs("Ошибка остановки частотного преобразователя!");
                                                                                                             }
                                                                                                         }};
                                                                                                     }}};

    // Парсинг команд из строки сценария
    std::istringstream stream(scenario);
    std::string command;
    while (std::getline(stream, command, ';')) {
        std::istringstream cmd_stream(command);
        std::string type;
        std::getline(cmd_stream, type, ':');

        std::vector<std::string> args;
        std::string arg;
        while (std::getline(cmd_stream, arg, ':')) {
            args.push_back(arg);
        }

        // Поиск соответствующего обработчика
        auto it = handlers.find(type);
        if (it != handlers.end()) {
            current_scenario.push_back(it->second(args));
        } else {
            throw std::invalid_argument("Unknown instruction type: " + type);
        }
    }

    Utils::take_logs("Сценарий интерпретирован успешно!");
    return current_scenario;
}

std::string Utils::read_from_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open the file: " + filename);
    }
    std::string scenario;
    std::getline(file, scenario);
    file.close();
    return scenario;
}

std::map<std::string, std::string> Utils::load_scenarios(const std::vector<std::string> &filenames) {
    std::map<std::string, std::string> scenarios;

    for (const auto &file : filenames) {
        try {
            std::string scenario = Utils::read_from_file("./scenarios/" + file);
            scenarios[file] = scenario;
            take_logs("Сценарий из файла " + file + " прочитан успешно!");
        } catch (const std::exception &e) {
            take_logs("Ошибка прочтения сценария из файла " + file + "!");
        }
    }

    return scenarios;
}

std::vector<std::string> Utils::get_files(const std::string &path) {
    std::vector<std::string> files;
    int count = 0;
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            // Используем .filename() вместо .path().string()
            files.push_back(entry.path().filename().string());
            count++;
        }
    }
    take_logs(std::to_string(count) + " файла сценариев загружено!");
    return files;
}

std::string _log;

std::string get_log() {
    return _log;
}

std::string Utils::hz_to_km(int hz) {
    std::string km = std::to_string(hz);
    return km;
}

void Utils::float_to_char_array(char *a, float num) {  // конвертация значения для строки (нагрузка)
    a[11] = 0x0A;
    sprintf(&a[0], "%F", num);
    for (int i = 0; a[i] != '\0'; i++) {
        if (a[i] == '.') {
            a[i] = ',';
        }
    }
}

std::string Utils::convert_float(float value, char mode) {  // конвертация значения для отправки на бп
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

float Utils::convert_string(std::string value2) {  // конвертация строки для считывания значения с бп
    float converted = 0.0f; // Значение по умолчанию
    size_t pos_v = value2.find("rv");
    size_t pos_i = value2.find("ri");

    try {
        if (pos_v != std::string::npos) {
            value2.erase(pos_v, 2); // Удаляем подстроку "rv"
            converted = std::stof(value2) / 10; // Преобразуем строку в число
        } else if (pos_i != std::string::npos) {
            value2.erase(pos_i, 2); // Удаляем подстроку "ri"
            converted = std::stof(value2) / 10; // Преобразуем строку в число
        }
    } catch (const std::invalid_argument& e) {
        // Обработка случая, когда строка не может быть преобразована в число
        std::cerr << "Invalid argument: Unable to convert string to float. Error: " << e.what() << std::endl;
        return 0.0f; // Возвращаем значение по умолчанию
    } catch (const std::out_of_range& e) {
        // Обработка случая, когда значение выходит за пределы допустимого диапазона
        std::cerr << "Out of range: Value is too large or too small to fit in a float. Error: " << e.what() << std::endl;
        return 0.0f; // Возвращаем значение по умолчанию
    }

    return converted;
}

std::string Utils::extract_tag(std::string &buffer, const std::string &tag) {
    std::string openTag = "<" + tag + ">";
    std::string closeTag = "</" + tag + ">";

    size_t start = buffer.find(openTag);
    if (start == std::string::npos) {
        throw std::runtime_error("Тег " + tag + " не найден.");
    }

    start += openTag.length();  // Переходим после открывающего тега
    size_t end = buffer.find(closeTag, start);

    if (end == std::string::npos) {
        throw std::runtime_error("Закрывающий тег для " + tag + " не найден.");
    }

    std::string value = buffer.substr(start, end - start);  // Извлекаем значение
    buffer = buffer.substr(end + closeTag.length());        // Удаляем уже обработанную часть

    return value;
}

long Utils::timediff_ms_from_now(struct timeval *tv) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec - tv->tv_sec) * 1000 + (now.tv_usec - tv->tv_usec) / 1000;
}
