#include "plugin.h"

//реализация базового класса-интерфейса

class BasePluginImpl : public BasePlugin {
  public:
    void read() override {} //для переопределения вирутальных методов родительского класса-интерфейса
    void connect() override {}
    void write() override {}
};

extern "C" BasePlugin *Create_plugin() { return new BasePluginImpl; }

extern "C" void Destroy_plugin(BasePlugin *plugin) { delete plugin; }