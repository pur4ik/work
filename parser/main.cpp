#include "buev.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

Buev uart("/dev/ttyUSB0", 115200); // 8бит четность? flow control

int main() {
    int count = 0;
    // sleep (1);
    // uart.write_data ("7");
    // sleep (1);

    //uart.write_data("t");

    while (true) {
        uart.read_data(1000);
        uart.start_parse(uart.buev_log);

        std::cout << "Parsed Data: ";
        std::cout << "time: " << uart.data.time << " ";
        std::cout << "fwv: " << uart.data.fwv << " ";
        std::cout << "rsv: " << uart.data.rsv << " ";
        std::cout << "spf: " << uart.data.spf << " ";
        std::cout << "iov: " << uart.data.iov << " ";
        std::cout << "ur: " << uart.data.ur << " ";
        std::cout << "up: " << uart.data.up << " ";
        std::cout << "uo: " << uart.data.uo << " ";
        std::cout << "cp: " << uart.data.cp << " ";
        std::cout << "u24: " << uart.data.u24 << " ";
        std::cout << "f: " << uart.data.f << " ";
        std::cout << "r: " << uart.data.r << " ";
        std::cout << "pm: " << uart.data.pm << " ";
        std::cout << "pt: " << uart.data.pt << " ";
        std::cout << "pr: " << uart.data.pr << " ";
        std::cout << "div: " << uart.data.div << " ";
        std::cout << "spi: " << uart.data.spi << " ";
        std::cout << "wm: " << uart.data.wm << " ";
        std::cout << "test: " << uart.data.test << " ";
        std::cout << "ig: " << uart.data.ig << " ";
        std::cout << "iab: " << uart.data.iab << " ";
        std::cout << "uab: " << uart.data.uab << " ";
        std::cout << "fw: " << uart.data.fw << " ";
        std::cout << "zp: " << uart.data.zp << " ";         // нулевая точка АЦП тока (26700 - флешка не прочитана)
        std::cout << "corph1: " << uart.data.corph1 << " "; // корректировка фаз
        std::cout << "corph2: " << uart.data.corph2 << " ";
        std::cout << "corph3: " << uart.data.corph3 << " ";
        std::cout << "pha: " << uart.data.pha << " ";
        std::cout << "phb: " << uart.data.phb << " ";
        std::cout << "phc: " << uart.data.phc << " ";
        std::cout << "temp: " << uart.data.temp << std::endl;

        count++;
        if (count == 5) {
            uart.write_data("7");
        }
        if (count == 10) {
            uart.write_data("8");
        }
        if (count == 15) {
            uart.write_data("9");
            count = 0;
        }
    }
    return 0;
}
