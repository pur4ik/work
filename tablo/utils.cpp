#include "utils.h"
#include <map>
#include <net/if.h>
#include <sstream>

namespace Utils {

std::vector<std::string> getNetworkInterfaces(bool all_ifaces) {
  std::vector<std::string> interfaces;
  struct ifaddrs *ifap = nullptr;

  if (getifaddrs(&ifap) != 0) {
    throw std::runtime_error("Не удалось получить список интерфейсов.");
  }

  for (struct ifaddrs *ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr && (ifa->ifa_flags & IFF_UP)) { // Только активные интерфейсы
      std::string iface_name(ifa->ifa_name);          // Преобразуем в std::string
      if (std::find(interfaces.begin(), interfaces.end(), iface_name) == interfaces.end()) {
        interfaces.push_back(iface_name); // Добавляем только уникальные интерфейсы
      }
    }
  }

  std::string elementToAdd = " ";
  interfaces.insert(interfaces.begin(), elementToAdd);

  if (!all_ifaces) {
    std::string elementToRemove = "lo";
    interfaces.erase(std::remove(interfaces.begin(), interfaces.end(), elementToRemove), interfaces.end());
  }

  freeifaddrs(ifap);
  return interfaces;
}

ParsedData parseAndDisplay(const std::string &input) {
  ParsedData data; // создание объекта структуры

  std::string str = (input[0] == '@') ? input.substr(1) : input; // убираем @

  std::stringstream ss(str); // создание строкового потока
  std::string pair;          // создание пары для хранения ключ-значение

  while (std::getline(ss, pair, ',')) {              // цикл по всем подстрокам разделенным ","
    size_t equalPos = pair.find('=');                // поиск "="
    if (equalPos != std::string::npos) {             // продолжение обработки
      std::string key = pair.substr(0, equalPos);    // извлечение ключа
      std::string value = pair.substr(equalPos + 1); // извлечение значения
        //сопоставление ключа и значения, сохранение значения в переменную
      if (key == "CarNum")
        data.carNum = value;
      else if (key == "Lang")
        data.lang = value;
      else if (key == "IDGK0")
        data.idGK0 = value;
      else if (key == "IDGK1")
        data.idGK1 = value;
      else if (key == "TempIn")
        data.tempIn = value;
      else if (key == "TempOut")
        data.tempOut = value;
      else if (key == "WC1")
        data.wc1 = value;
      else if (key == "WC2")
        data.wc2 = value;
      else if (key == "Alarm")
        data.alarm = value;
      else if (key == "Delay")
        data.delay = value;
      else if (key == "DateTime")
        data.dateTime = value;
      else if (key == "Call")
        data.call = value;
      else if (key == "Rest")
        data.rest = value;
      else if (key == "Speed")
        data.speed = value;
      else if (key == "Position")
        data.position = value;
      else if (key == "Stations")
        data.stations = value;
    }
  }

  return data;
}
} // namespace Utils