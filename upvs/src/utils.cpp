#include "utils.h"

Utils util;

std::string Utils::extract_tag(std::string& buffer, const std::string& tag) {
    std::string openTag = "<" + tag + ">";
    std::string closeTag = "</" + tag + ">";

    size_t start = buffer.find(openTag);
    if (start == std::string::npos) {
        throw std::runtime_error("Тег " + tag + " не найден.");
    }

    start += openTag.length(); // Переходим после открывающего тега
    size_t end = buffer.find(closeTag, start);

    if (end == std::string::npos) {
        throw std::runtime_error("Закрывающий тег для " + tag + " не найден.");
    }

    std::string value = buffer.substr(start, end - start); // Извлекаем значение
    buffer = buffer.substr(end + closeTag.length()); // Удаляем уже обработанную часть

    return value;
}