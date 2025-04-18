#include "psi.h"

#include <chrono>
#include <thread>

Psi psi;

int Psi::init() {
    std::cout << "INIT\n";
    if (bp.init() == 0 && visa_device.visa_connection == true /*&& _socket.load_connection && modbus.modbus_connection*/) {
        std::cout << "БП+ММ ОК\n";
        if (bp.out_on() == 0) {
            while (!buevg_udp.bm_connection) {
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                std::cout << "ожидаю инициализации БМ \n";
            }
            std::cout << "БМ ОК\n";
            while (!upvs_request.upvs_connection) {
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                std::cout << "ожидаю инициализации УПВС \n";
            }
            std::cout << "УПВС ОК\n";
        } else {
            std::cout << "Ошибка включения БП \n";
            return -1;
        }
    } else {
        std::cout << "Ошибка глобальной инциализации\n";
        return -1;
    }
    return 0;
}

int Psi::_220(psi_results &results) {
    loader.set_load(0.0);
    std::cout << "послал 0 нагр\n";

    while (!flags._220_start) {
    }
    std::cout << "выставил 50 на ЧП\n";
    std::cout << "стартанул\n";
    // test2.freq_change(5000);
    // test2.start();
    while (flags._220_1) {
    }
    // test2.stop();
    std::cout << "остановился 220\n";
    std::cout << "results : " << results._220_1 << " " << results._220_2 << "\n";
    return 0;
}

int Psi::_221(psi_results &results) {
    loader.set_load(50.0);
    std::cout << "послал 50 нагр\n";
    while (!flags._221_start) {
    }
    std::cout << "выставил 50 на ЧП\n";
    std::cout << "стартанул\n";
    // test2.freq_change(5000);
    // test2.start();
    while (flags._221_1) {
    }
    std::cout << "остановился 221\n";
    std::cout << "results : " << results._221_1 << " " << results._221_2 << "\n";
    return 0;
}

int Psi::_224 (psi_results &results, upvs_data &data){
    std::cout << "224 starting\n";
             bp.set_u(120.0);
            if (data.POWER_GENERATOR == -5000.0 || data.U_GENERATOR == -5000.0 || data.U_WINDING_GENERATOR == -5000.0 || data.I_WINDING_GENERATOR == -5000.0 ||
                data.SPEED_BY_GENERATOR == -5000.0 || data.U_110 == -5000.0 || data.I_110 == -5000.0 || data.I_BATTERY == -5000.0 || data.BATTERY_VOLTAGE == -5000.0 ||
                data.TEMP_BATTERY == -5000.0) {
                results._224 = -2;  // Найдено недопустимое значение
                std::cout << "224 завершен с ошибкой" << "\n";
                return 0;
            }

            if (data.STATUS == "-" || data.RCH == "-" || data.RMT_Z == "-" || data.RMT_R == "-" || data.BATTERY_CIRCUIT_BREAKAGE == "-" || data.RPN1 == "-" || data.RPN2
            == "-" ||
                data.TYPE_AKK == "-" || data.TYPE_GEN == "-") {
                    results._224 = false;  // Найдено недопустимое значение
                    std::cout << "224 завершен с ошибкой" << "\n";
                    return 0;
            }
            results._224 = 0;
            std::cout << "224 завершен" << "\n";
            
            return 0;
}           

int Psi::_225 (psi_results &results, upvs_data &data) {
    std::cout << "225 starting\n";
            bp.set_u_kp(100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if (abs(100.0 - akip.read_u()) < 0.1) {
                results._225 = data.BATTERY_VOLTAGE;
            } else {
                std::cout << "не соответствие значений кп\n";
            }
            std::cout << "225 завершен" << "\n";
            return 0;
}