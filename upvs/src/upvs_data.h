#include <string>
struct upvs_data {
    double POWER_GENERATOR = 0.0;
    double U_GENERATOR = 0.0;
    double U_WINDING_GENERATOR = 0.0;
    double I_WINDING_GENERATOR = 0.0;
    double SPEED_BY_GENERATOR = 0.0;
    int GENERATOR_SHAFT_ROTATION_SPEED = 0;
    double U_110 = 0.0;
    double I_110 = 0.0;
    double I_BATTERY = 0.0;
    int BATTERY_CHARGE_STAGE = 0;
    double BATTERY_VOLTAGE = 0.0;
    double TEMP_BATTERY = 0.0;

    std::string STATUS = "UNKNOWN";
    std::string RCH = "NO";
    std::string RMT_Z = "NO";
    std::string RMT_R = "NO";
    std::string BATTERY_CIRCUIT_BREAKAGE = "NO";
    std::string RPN1 = "NO";
    std::string RPN2 = "NO";

    std::string TYPE_AKK = "UNKNOWN";
    std::string TYPE_GEN = "UNKNOWN";
};