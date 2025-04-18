
#include <modbus/modbus.h>

#include <iostream>
#include <string>

#pragma once

class ModBus {
   private:
    std::string _port;
    int _baud;
    char _parity;
    int _data_bit;
    int _stop_bit;
    int _slave_address;

   public:
    bool modbus_connection = false;
    modbus_t *_ctx;
    ModBus(const std::string &port, int baud, char parity, int data_bit, int stop_bit, int slave_address);
    ~ModBus();
    int read_param(const std::string &adr, bool log = false);
    int write_param(const std::string &adr, uint16_t value, bool log = false);
    int packets = 0;
    int error_packets = 0;
};
extern ModBus modbus;
