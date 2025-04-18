#include "data.h"

Data test;

std::string Data::read_file(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return "";
    }

    std::string firstLine;
    std::getline(infile, firstLine); // Читаем первую строку

    // Копируем оставшийся контент во временный файл
    std::ostringstream tempContent;
    std::string line;
    while (std::getline(infile, line)) {
        tempContent << line << "\n";
    }
    infile.close();

    // Записываем временный контент обратно в исходный файл
    std::ofstream outfile(filename, std::ios::trunc);
    if (outfile.is_open()) {
        outfile << tempContent.str();
        outfile.close();
    } else {
        std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
    }

    std::cout<<"считано" << firstLine << "\n";  

    return firstLine;
}

void Data::parse_response(const std::string& response, upvs_data& data) {
    std::istringstream stream(response); // Создаём поток из строки
    std::string line;

    // Вспомогательная функция для извлечения значения между <value>...</value>
    auto extractValue = [&stream]() -> std::string {
        std::string buffer;
        size_t start = buffer.find("<value>");
        size_t end = buffer.find("</value>");
        if (start != std::string::npos && end != std::string::npos) {
            return buffer.substr(start + 7, end - start - 7); // 7 = длина "<value>"
        }
        return "";
    };

    // Вспомогательная функция для безопасного преобразования строк в числа
    auto safe_stod = [](const std::string& value, const std::string& fieldName) -> double {
        try {
            return std::stod(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка преобразования '" << value << "' для поля " << fieldName << ": " << e.what() << std::endl;
            return 0.0;
        } catch (const std::out_of_range& e) {
            std::cerr << "Значение '" << value << "' для поля " << fieldName << " вне допустимого диапазона: " << e.what() << std::endl;
            return 0.0;
        }
    };
    auto safe_stoi = [](const std::string& value, const std::string& fieldName) -> int {
        try {
            return std::stoi(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка преобразования '" << value << "' для поля " << fieldName << ": " << e.what() << std::endl;
            return 0;
        } catch (const std::out_of_range& e) {
            std::cerr << "Значение '" << value << "' для поля " << fieldName << " вне допустимого диапазона: " << e.what() << std::endl;
            return 0;
        }
    };
    try {
        std::string buffer = response; // Копируем всю строку для обработки
        data.POWER_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "POWER_GENERATOR");
        data.U_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "U_GENERATOR");
        data.U_WINDING_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "U_WINDING_GENERATOR");
        data.I_WINDING_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "I_WINDING_GENERATOR");
        data.SPEED_BY_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "SPEED_BY_GENERATOR");
        data.GENERATOR_SHAFT_ROTATION_SPEED = safe_stoi(util.extract_tag(buffer, "value"), "GENERATOR_SHAFT_ROTATION_SPEED");
        data.U_110 = safe_stod(util.extract_tag(buffer, "value"), "U_110");
        data.I_110 = safe_stod(util.extract_tag(buffer, "value"), "I_110");
        data.I_BATTERY = safe_stod(util.extract_tag(buffer, "value"), "I_BATTERY");
        data.BATTERY_CHARGE_STAGE = safe_stoi(util.extract_tag(buffer, "value"), "BATTERY_CHARGE_STAGE");
        data.BATTERY_VOLTAGE = safe_stod(util.extract_tag(buffer, "value"), "BATTERY_VOLTAGE");
        data.TEMP_BATTERY = safe_stod(util.extract_tag(buffer, "value"), "TEMP_BATTERY");

        data.STATUS = util.extract_tag(buffer, "value");
        data.RCH = util.extract_tag(buffer, "value");
        data.RMT_Z = util.extract_tag(buffer, "value");
        data.RMT_R = util.extract_tag(buffer, "value");
        data.BATTERY_CIRCUIT_BREAKAGE = util.extract_tag(buffer, "value");
        data.RPN1 = util.extract_tag(buffer, "value");
        data.RPN2 = util.extract_tag(buffer, "value");

        data.TYPE_AKK = util.extract_tag(buffer, "value");
        data.TYPE_GEN = util.extract_tag(buffer, "value");
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при парсинге ответа: " << e.what() << std::endl;
    }
}
