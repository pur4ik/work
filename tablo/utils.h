#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <stdexcept>
#include <ifaddrs.h>
#include <algorithm>

namespace Utils {
struct ParsedData {
  std::string carNum;
  std::string lang;
  std::string idGK0;
  std::string idGK1;
  std::string tempIn;
  std::string tempOut;
  std::string wc1;
  std::string wc2;
  std::string alarm;
  std::string delay;
  std::string dateTime;
  std::string call;
  std::string rest;
  std::string speed;
  std::string position;
  std::string stations;
};
    std::vector<std::string> getNetworkInterfaces(bool all_ifaces);
    ParsedData parseAndDisplay(const std::string &input);
}

#endif // UTILS_H