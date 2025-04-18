#include "modbus.h"

#pragma once

class Freq_converter {
   private:
    ModBus& modbus;
    float current_freq;
    int frequency;
    int time;

   public:
   bool timer = false;
   int on;
   int work;
    Freq_converter(ModBus& modbus) : modbus(modbus) {}
    int freq_change(int frequency, float time = 0);
    int prepare();
    int unprepare();
    int start();
    int stop();
    void work_time();
    bool alarm = false;
    bool error = false;
    std::string condition();
    void sleep(int duration);
    bool ready();
    int get_freq(bool log = false) {
        current_freq = modbus.read_param("3001", log);  // параметр считывания текущей частоты 0528 чтение текущей заданной частоты
                                                      //   std::cout << "СЧИТАЛ ЧАСТОТУ:"<< current_freq<<std::endl;
        return current_freq;
    };
};
extern Freq_converter test2;