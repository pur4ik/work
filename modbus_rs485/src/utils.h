#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/time.h>
#include <vector>

#include "freq/freq_converter.h"
#include "modbus.h"

extern Freq_converter test2;

class Utils {
   public:
    std::string convert_float(float value, char mode);
    float convert_string(std::string value);

    struct Instruction {
        std::function<void()> func;
    };

    std::string extract_tag(std::string& buffer, const std::string& tag);
    void float_to_char_array(char* a, float num);
    std::vector<Instruction> parse_scenario(const std::string& scenario);
    int convert(const std::string& adr);
    std::string read_from_file(const std::string& filename);
    std::map<std::string, std::string> load_scenarios(const std::vector<std::string>& filenames);
    std::vector<std::string> get_files(const std::string& path);
    std::string hz_to_km(int hz);

    long timediff_ms_from_now(struct timeval *tv);

    void take_logs(const std::string& log, char type = 'm');
    std::string get_log();
    std::string _log;
};  // namespace Utils
extern Utils util;
#endif