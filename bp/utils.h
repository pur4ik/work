#include <string>
class Utils {
    public: //конвертация float в строку для выставления
    std::string convert_float (float value, char mode);
    float convert_string (std::string value);
};
extern Utils util;