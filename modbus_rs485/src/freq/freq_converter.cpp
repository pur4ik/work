#include "freq_converter.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "../utils.h"

Freq_converter test2(modbus);

int Freq_converter::freq_change(int freq, float time) {
    int step;
    int error_count = 0;
    int current_freq = get_freq(false);
    int start_freq = current_freq;
    error = false;
    alarm = false;
    int var = std::abs(freq - start_freq);
    if (freq > current_freq) {
        step = 50;
        util.take_logs("Разгон с " + std::to_string(current_freq / 108) + " до " + std::to_string(freq / 108) + " за " + std::to_string(time / 1000) + " секунд");
    } else {
        if (var == 0) {
            return current_freq;
        }
        step = -50;
        util.take_logs("Торможение с " + std::to_string(current_freq / 108) + " до " + std::to_string(freq / 108) + " за " + std::to_string(time / 1000) + " секунд");
    }
    int time_step = std::abs((time / var) * step);
    //std::cout << "time step:" << time_step << std::endl;
    while ((start_freq != freq) && (error_count <= 3) && (!alarm)) {
        //std::cout << "SEND" << std::endl;
        var = freq - start_freq;
        if (std::abs(var) < std::abs(step)) {
            step = var;
        }
        //std::cout << "ошибок: " << error_count << std::endl;
        auto start_time = std::chrono::steady_clock::now();  // фиксируем время начала итерации
        //std::cout << "ЗАСЫЛАЕМ: " << start_freq + step << std::endl;
        if (modbus.write_param("3001", start_freq + step, false) == 0) {
            error_count = 0;
            start_freq += step;
            auto end_time = std::chrono::steady_clock::now();  // фиксируем время окончания операций
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            if (elapsed_time < time_step) {
                std::this_thread::sleep_for(std::chrono::milliseconds(time_step - elapsed_time));
            }
        } else {
            error_count++;
            continue;
        }
    }
    if ((error_count >= 6) || (alarm == true)) {
        error = true;
        return -1;
    }
    return start_freq;
}

void Freq_converter::sleep(int duration) {
    util.take_logs("Пауза " + std::to_string(duration / 1000) + " секунд");
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

bool Freq_converter::ready() {
    if (modbus.read_param("0115", false) == 2 && modbus.read_param("0100", false) == 4) {
        return true;
    } else {
        return false;
    }
}

void Freq_converter::work_time() {
    on = modbus.read_param("0538");
    work = modbus.read_param("0539");
}

int Freq_converter::prepare() {
    int result1 = modbus.write_param("0115", 2, false);
    int result2 = modbus.write_param("0100", 4, false);
    return result1 || result2;
}

int Freq_converter::unprepare() {
    int result1 = modbus.write_param("0115", 1, false);
    int result2 = modbus.write_param("0100", 1, false);
    return result1 || result2;
}

int Freq_converter::start() {

    timer = true;

    int start_error_count = 0;
    int result = -1;
    do {
        result = modbus.write_param("3000", 5, false);
        if (result != 0) {
            start_error_count++;
        }
    } while ((start_error_count <= 6) && (result != 0));
    if (start_error_count < 6) {
        return 0;
    } else {
        return -1;
    }
}

int Freq_converter::stop() {
    timer = false;
    int stop_error_count = 0;
    int result = -1;
    do {
        result = modbus.write_param("3000", 6, false);
        if (result != 0) {
            stop_error_count++;
        }
    } while ((stop_error_count <= 6) && (result != 0));
    if (stop_error_count < 6) {
        return 0;
    } else {
        return -1;
    }
}

std::string Freq_converter::condition() {
    return std::to_string(modbus.read_param("3002", false));
}