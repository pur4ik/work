#include "upvs.h"

Upvs upvs_request;

void Upvs::recieve_data(upvs_data &data) {
    commander.command();
    data_obj.parse_response(data_obj.read_file(commander.response_file), data);
    if (data.POWER_GENERATOR == -5000.0 || data.U_GENERATOR == -5000.0 || data.U_WINDING_GENERATOR == -5000.0 || data.I_WINDING_GENERATOR == -5000.0 ||
        data.SPEED_BY_GENERATOR == -5000.0 || data.U_110 == -5000.0 || data.I_110 == -5000.0 || data.I_BATTERY == -5000.0 || data.BATTERY_VOLTAGE == -5000.0 ||
        data.TEMP_BATTERY == -5000.0) {
        upvs_connection = false;  // Найдено недопустимое значение
    }

    if (data.STATUS == "-" || data.RCH == "-" || data.RMT_Z == "-" || data.RMT_R == "-" || data.BATTERY_CIRCUIT_BREAKAGE == "-" || data.RPN1 == "-" || data.RPN2 == "-" ||
        data.TYPE_AKK == "-" || data.TYPE_GEN == "-") {
        upvs_connection = false;  // Найдено недопустимое значение
    }
    upvs_connection = true;
}

void Upvs::stop() {
    commander.exit();
}