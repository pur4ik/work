#ifndef MYVISA_H
#define MYVISA_H

#include <iostream>
#include "visa.h"

class MyVisa {
   private:
    const char *resource = "USB0::0x164E::0x0DB7::TW00039246::INSTR";
    ViSession defaultRM;
    ViSession vi;
    ViStatus status;
    char buffer[256];

   public:
    bool visa_connection;

    MyVisa() {
        status = viOpenDefaultRM(&defaultRM);
        if (status != VI_SUCCESS) {
            std::cerr << "Ошибка инициализации VISA: " << status << std::endl;
            visa_connection = false;
        }

        // Открытие соединения с устройством
        status = viOpen(defaultRM, (ViRsrc)resource, VI_NULL, VI_NULL, &vi);
        if (status != VI_SUCCESS) {
            std::cerr << "Не удалось открыть устройство: " << status << std::endl;

            // Расшифровка ошибки
            switch (status) {
                case VI_ERROR_RSRC_NFOUND:
                    std::cerr << "Ошибка: Устройство не найдено" << std::endl;
                    break;
                case VI_ERROR_INV_RSRC_NAME:
                    std::cerr << "Ошибка: Неверное имя ресурса" << std::endl;
                    break;
                default:
                    std::cerr << "Неизвестная ошибка" << std::endl;
            }

            visa_connection = false;

            viClose(defaultRM);
        } else {
            visa_connection = true;
            std::cout << "Соединение с мультиметром установлено успешно" << "\n";
        }
    }

    ~MyVisa() {
        viClose(vi);
        viClose(defaultRM);
    }

    // чтение
    int visa_write(const char *command);
    std::string visa_read();
};

extern MyVisa visa_device;
#endif