#include "plugin.h"
#include <iostream>

class PluginMB : public BasePlugin {
  public:
    PluginMB() { std::cout << "Создание ModBus соединения\n"; }  // создание соединения можно разместить в конструкторе класса
    ~PluginMB() { std::cout << "Закрытие ModBus соединения\n"; } // закрытие соединения можно разместить в деструкторе класса

    // внутренние функции плагина обеспечивающие его функционал (переопределение базовых виртуальных методов объявленных в родительском классе BasePlugin)
    void connect() { std::cout << "соединяюсь по ModBus \n"; }
    void write() {
        std::cout << "пишу в ModBus \n";
        extended_logic();
    }
    void read() { std::cout << "читаю по ModBus \n"; }

    void extended_logic() { std::cout << "дополнительный функционал (вызов сторонней функции класса) класса ModBus в базовом методе write \n"; }
};

// реализация фабричной функции из базового класса для создания объекта класса PluginMB
// компиляция в С-стиле для того чтобы функция была определена dlsym
// полиморфизм позволяет обращаться к объекту класса PluginMB через указатель на объект родительского класса BasePlugin
extern "C" BasePlugin *Create_plugin() { return new PluginMB; }


// реализация фабричной функции из базового класса для удаления объекта класса PluginMB
// так как в родительском классе деструктор виртуальный, будет выполняться операции из деструктора дочернего класса PluginMB
extern "C" void Destroy_plugin(BasePlugin *plugin) { delete plugin; }