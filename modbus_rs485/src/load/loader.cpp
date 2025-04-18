#include "loader.h"

Loader loader;

int Loader::set_load(float value) {
    util.float_to_char_array (str, value );
    if (_socket.send(str) == 0) {
        if (_socket.listen_reply() == 0) {
            std::cout << "значение нагрузки " << value << " % отправлено успешно!\n";
            return 0;
        }
        else {
            std::cout << "корректный ответ не получен \n";
            return -1;
        }
    }
    else {
        std::cout << "отправка значения завершилось ошибкой \n";
            return -1;
    }
}