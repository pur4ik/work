#include "modbus.h"

#include <unistd.h>

#include "../utils.h"

ModBus modbus("/dev/ttyACM0", 9600, 'N', 8, 1, 1);

ModBus::ModBus(const std::string &port, int baud, char parity, int data_bit, int stop_bit, int slave_address)
    : _port(port), _baud(baud), _parity(parity), _data_bit(data_bit), _stop_bit(stop_bit), _slave_address(slave_address), modbus_connection(false) {
        //std::cout << "1"<<std::endl;
    _ctx = modbus_new_rtu(port.c_str(), baud, parity, data_bit, stop_bit);
    //std::cout << "2"<<std::endl;
    if (_ctx == nullptr) {
        std::cerr << "Ошибка создания контекста: " << modbus_strerror(errno) << std::endl;
        util.take_logs("Ошибка при установке ModBus соединения!");
        return;  // Просто выходим из конструктора, не завершая программу
    }

    if (modbus_connect(_ctx) == -1) {
        std::cerr << "Ошибка соединения: " << modbus_strerror(errno) << std::endl;
        util.take_logs("Ошибка при установке ModBus соединения!");
        modbus_free(_ctx);
        _ctx = nullptr;
        return;  // Выходим из конструктора, не завершая программу
    }

    modbus_set_slave(_ctx, slave_address);
    //std::cout << "ctx при окончании конструктора: " << _ctx << std::endl;
    util.take_logs("ModBus соединение установлено успешно!");

    modbus_connection = true;  // Если соединение успешно, модуль подключен
}



ModBus::~ModBus() {
    if (_ctx != nullptr) {
        modbus_close(_ctx);
        modbus_free(_ctx);
    }
}

int ModBus::read_param(const std::string &adr, bool log) {
    std::string group = adr.substr(0, 2);
    std::string param = adr.substr(2, 2);

    uint16_t regs[1];
    int rc = modbus_read_registers(_ctx, util.convert(adr), 1, regs);

    if (rc == -1) {
        error_packets++;
        // std::cerr << "Ошибка чтения: " << modbus_strerror(errno) << std::endl;
        if (log) {
            util.take_logs("Ошибка при выполнении операции чтения!");
        }
        return -1;
    } else {
        if (log) {
            util.take_logs("Параметр F." + group + "." + param + " = " + std::to_string(regs[0]));
        }
        packets++;
        return regs[0];
    }
}

int ModBus::write_param(const std::string &adr, uint16_t value, bool log) {
    int rc = modbus_write_register(_ctx, util.convert(adr), value);
    if (rc == -1) {
        error_packets++;
        std::cerr << "Ошибка записи: " << modbus_strerror(errno) << std::endl;
        util.take_logs("Ошибка записи параметра!");
        return -1;
    } else {
        if (log) {
            util.take_logs("Параметр записан успешно!");
            read_param(adr, true);
        }
        packets++;
        return 0;
    }
}

// для функции изменения параметра необходимы проверки:
//-не является ли параметр неизменяемым
//-допустимо ли изменение параметра в режиме работы (соотв находится ли чп в режиме работы в данный момент)
// предполагается, что запуск преобразователя также будет осуществляться с помощью изменения параметров, следовательно bool переменная для отслеживания состояния чп меняющаяся при
// пуске/остановке чп