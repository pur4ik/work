#include <iostream>
#include <string>
#include <unistd.h> 
#include "data.h"
#include "upvs.h"
#include "netcat.h"

/*

sudo ifconfig enp0s31f6 10.7.8.231 netmask 255.255.0.0 up
sudo ip route add 10.7.8.0/24 dev enp0s31f6

sudo ifconfig enp0s31f6 192.168.5.208 netmask 255.255.0.0 up
*/

upvs_data data;
//Upvs upvs_request;

int main() {


    while (true) {
        upvs_request.recieve_data(data);

        // Выводим текущие данные
        std::cout << "POWER_GENERATOR: " << data.POWER_GENERATOR << " кВт" << std::endl;
        std::cout << "U_GENERATOR: " << data.U_GENERATOR << " В" << std::endl;
        std::cout << "U_WINDING_GENERATOR: " << data.U_WINDING_GENERATOR << " В" << std::endl;
        std::cout << "I_WINDING_GENERATOR: " << data.I_WINDING_GENERATOR << " А" << std::endl;
        std::cout << "SPEED_BY_GENERATOR: " << data.SPEED_BY_GENERATOR << std::endl;
        std::cout << "GENERATOR_SHAFT_ROTATION_SPEED: " << data.GENERATOR_SHAFT_ROTATION_SPEED << " об/мин" << std::endl;
        std::cout << "U_110: " << data.U_110 << " В" << std::endl;
        std::cout << "I_110: " << data.I_110 << " А" << std::endl;
        std::cout << "I_BATTERY: " << data.I_BATTERY << " А" << std::endl;
        std::cout << "BATTERY_CHARGE_STAGE: " << data.BATTERY_CHARGE_STAGE << std::endl;
        std::cout << "BATTERY_VOLTAGE: " << data.BATTERY_VOLTAGE << " В" << std::endl;
        std::cout << "TEMP_BATTERY: " << data.TEMP_BATTERY << " °C" << std::endl;
    
        std::cout << "STATUS: " << data.STATUS << std::endl;
        std::cout << "RCH: " << data.RCH << std::endl;
        std::cout << "RMT_Z: " << data.RMT_Z << std::endl;
        std::cout << "RMT_R: " << data.RMT_R << std::endl;
        std::cout << "BATTERY_CIRCUIT_BREAKAGE: " << data.BATTERY_CIRCUIT_BREAKAGE << std::endl;
        std::cout << "RPN1: " << data.RPN1 << std::endl;
        std::cout << "RPN2: " << data.RPN2 << std::endl;
    
        std::cout << "TYPE_AKK: " << data.TYPE_AKK << std::endl;
        std::cout << "TYPE_GEN: " << data.TYPE_GEN << std::endl;

        std::cout << "\n\n\n";

        // Ждём 1 секунду перед следующим запросом
        sleep(1);
    }

    return 0;
}