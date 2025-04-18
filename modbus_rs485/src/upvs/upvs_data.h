#ifndef UPVS_DATA_H
#define UPVS_DATA_H

#include <string>
struct upvs_data {
    double POWER_GENERATOR = -5000.0;
    double U_GENERATOR = -5000.0;
    double U_WINDING_GENERATOR = -5000.0;
    double I_WINDING_GENERATOR = -5000.0;
    double SPEED_BY_GENERATOR = -5000.0;
    int GENERATOR_SHAFT_ROTATION_SPEED = -5000;
    double U_110 = -5000.0;
    double I_110 = -5000.0;
    double I_BATTERY = -5000.0;
    int BATTERY_CHARGE_STAGE = -5000;
    double BATTERY_VOLTAGE = -5000.0;
    double TEMP_BATTERY = -5000.0;

    std::string STATUS = "-";
    std::string RCH = "-";
    std::string RMT_Z = "-";
    std::string RMT_R = "-";
    std::string BATTERY_CIRCUIT_BREAKAGE = "-";
    std::string RPN1 = "-";
    std::string RPN2 = "-";

    std::string TYPE_AKK = "-";
    std::string TYPE_GEN = "-";
};
#endif