#include "myvisa.h"
#include <cstring>
#include <string>

MyVisa visa_device;

int MyVisa:: visa_write(const char* command) {
    status = viWrite(vi, (ViBuf)command, strlen(command), nullptr);
    if (status != VI_SUCCESS) {
        std::cerr << "Не удалось отправить команду" << status << std::endl;
        viClose(vi);
        viClose(defaultRM);
        return -1;
    }
    return 0;
}

std::string MyVisa::visa_read () {
    ViUInt32 bytes_read;
    status = viRead(vi, (ViPBuf)buffer, sizeof(buffer) - 1, &bytes_read);
    if (status == VI_SUCCESS && bytes_read > 0) {
        buffer[bytes_read] = '\0'; // Добавляем завершающий ноль

    } else {
        std::cerr << "Не удалось прочитать ответ от устройства" << std::endl;
    }
    std::string responce(buffer);
    return responce;
}