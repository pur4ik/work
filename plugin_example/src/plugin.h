
#ifndef PLUGIN_H
#define PLUGIN_H

#include <iostream>

class __attribute__((visibility("default"))) BasePlugin {
public:
    virtual ~BasePlugin() {}
    virtual void connect() = 0;
    virtual void read() = 0;
    virtual void write() = 0;
};

// Фабричные функции
typedef BasePlugin *(*Create_plugin_t)();
typedef void (*Destroy_plugin_t)(BasePlugin *);

#endif // PLUGIN_H