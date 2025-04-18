#include "mm.h"

Mm akip;

float Mm::read_u() {
  try {
      // Отправляем команду на измерение напряжения
      visa_device.visa_write(":MEASure:VOLTage:DC?");

      // Читаем ответ от устройства
      std::string response = visa_device.visa_read();

      // Преобразуем ответ в число
      float value = std::stof(response);

      // Сохраняем значение в переменную u
      u = value;

      // Возвращаем устройство в локальный режим
      visa_device.visa_write("SYST:LOC");

      // Возвращаем значение
      return value;
  } catch (const std::invalid_argument& e) {
      // Обработка случая, когда строка не может быть преобразована в число
      std::cerr << "Invalid argument: Unable to convert string to float. Response: '"
                << visa_device.visa_read() << "'. Error: " << e.what() << std::endl;
      return 0.0f; // Возвращаем значение по умолчанию
  } catch (const std::out_of_range& e) {
      // Обработка случая, когда значение выходит за пределы допустимого диапазона
      std::cerr << "Out of range: Value is too large or too small to fit in a float. Response: '"
                << visa_device.visa_read() << "'. Error: " << e.what() << std::endl;
      return 0.0f; // Возвращаем значение по умолчанию
  }
}

