#include "plugin.h"
#include <iostream>

class PluginUART : public BasePlugin {
  public:
    PluginUART() { std::cout << "Создание UART соединения\n"; }  // создание соединения можно разместить в конструкторе класса
    ~PluginUART() { std::cout << "Закрытие UART соединения\n"; } // закрытие соединения можно разместить в деструкторе класса

    // внутренние функции плагина обеспечивающие его функционал (переопределение базовых виртуальных методов объявленных в родительском классе BasePlugin)
    void connect() { std::cout << "соединяюсь по UART \n"; }
    void write() { std::cout << "пишу в UART \n"; }

    void read() { // функция с расширенным функционалом
        std::cout << "читаю по UART \n";
        extended_logic();
    }
    //дополнительная функция для расширения функционала базовых функций родительского класса 
    void extended_logic() { std::cout << "дополнительный функционал (вызов сторонней функции класса) класса UART в базовом методе read \n"; }
};

// реализация фабричной функции из базового класса для создания объекта класса PluginUART
// компиляция в С-стиле для того чтобы функция была определена dlsym
// полиморфизм позволяет обращаться к объекту класса PluginUART через указатель на объект родительского класса BasePlugin
extern "C" BasePlugin *Create_plugin() { return new PluginUART; }


// реализация фабричной функции из базового класса для удаления объекта класса PluginUART
// так как в родительском классе деструктор виртуальный, будет выполняться операции из деструктора дочернего класса PluginUART
extern "C" void Destroy_plugin(BasePlugin *plugin) { delete plugin; }