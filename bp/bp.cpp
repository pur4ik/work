#include "bp.h"
#include <chrono>
#include <cstring>
#include <thread>

Bp bp;

int Bp::init() {
    uart.uart_write("a\r\n");
    std::string responce = uart.uart_read();
    if (responce.length() != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("so0\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("su00000\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("si01000\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("bc00000\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("bs00000\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("bm00000\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("bn00000\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        uart.uart_write("pl\r\n");
        responce.clear();
        sleep (3);
        return 0;
    } else
        return -1;
}

void Bp::stop() {
    uart.uart_write("so0\r\n");
    uart.uart_write("pu\r\n");
    uart.uart_write("bc00000\r\n");
    uart.uart_write("bs00000\r\n");
    uart.uart_write("bm00000\r\n");
    uart.uart_write("bn00000\r\n");
}

int Bp::set_u(float u_value, float time) {
    float u_step;
    std::string u_command;
    if (time == 0.0) {
        u_command = util.convert_float(u_value, 'u');
        uart.uart_write(u_command.c_str());
        u_command.clear();
        sleep(1);

    } else {
        std::cout << "попал" << "\n";
        float current_u = read_u ();
        std::cout << "считал:" << current_u << "\n";
        float dif = u_value - current_u;
        if (dif >= 0) {
            u_step = 0.1;
        } else {
            u_step = -0.1;
        }
        dif = abs(dif);
        std::cout << "разница" << dif << "\n";
        float time_step = std::abs((time / dif) * u_step);
        std::cout << "time_step " << time_step << "\n";
        for (dif; dif > 0;) {
            auto start_time = std::chrono::steady_clock::now(); // фиксируем время начала итерации
            u_command = util.convert_float(current_u, 'u');
            std::cout << "отправил:" << u_command << "\n";
            uart.uart_write(u_command.c_str());
            auto end_time = std::chrono::steady_clock::now(); // фиксируем время окончания операций
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            if (elapsed_time < time_step) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(time_step) - elapsed_time));
            }
            dif -= abs(u_step);
            current_u += abs(u_step);
        }
    }
    uart.flush_uart();
    return 0;
}

int Bp::set_i(float i_value, int time) {
    float i_step;
    std::string i_command;
    if (time == 0) {
        i_command = util.convert_float(i_value, 'i');
        uart.uart_write(i_command.c_str());
    } else {
        std::cout << "попал" << "\n";
        float current_i = read_i ();
        std::cout << "считал:" << current_i << "\n";
        float dif = i_value - current_i;
        if (dif >= 0) {
            i_step = 0.1;
        } else {
            i_step = -0.1;
        }
        dif = abs(dif);
        std::cout << "разница" << dif << "\n";
        float time_step = std::abs((time / dif) * i_step);
        std::cout << "time_step " << time_step << "\n";
        for (dif; dif > 0;) {
            auto start_time = std::chrono::steady_clock::now(); // фиксируем время начала итерации
            i_command = util.convert_float(current_i, 'i');
            std::cout << "отправил:" << i_command << "\n";
            uart.uart_write(i_command.c_str());
            auto end_time = std::chrono::steady_clock::now(); // фиксируем время окончания операций
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            if (elapsed_time < time_step) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(time_step) - elapsed_time));
            }
            dif -= abs(i_step);
            current_i += abs(i_step);
        }
    }
    uart.flush_uart();
    return 0;
}

float Bp::read_u() {

    float u_value = 0;
    uart.flush_uart();
    uart.uart_write("rv\r\n");
    std::string u_value_str = uart.uart_read();
    u_value = util.convert_string(u_value_str);
    return u_value;
}

float Bp::read_i() {
    uart.flush_uart();
    uart.uart_write("ri\r\n");
    std::string i_value_str = uart.uart_read();
    float i_value = util.convert_string(i_value_str);
    return i_value;
}

int Bp::out_on (){
    if (uart.uart_write ("so1\r\n") == 0 ){
        return 0;
    }
    else {
        return -1;
    }
}

int Bp::out_off (){
    if (uart.uart_write ("so0\r\n") == 0 ){
        return 0;
    }
    else {
        return -1;
    }
}
