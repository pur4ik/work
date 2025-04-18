#include "plugin.h"
#include <dlfcn.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <vector>
#include <map>

typedef BasePlugin *(*Create_plugin_t)();
typedef void (*Destroy_plugin_t)(BasePlugin *);

//функция для опредедения доступных плагинов в директории
std::vector<std::string> scan_plugins_directory(const std::string &dir_path) {
    std::vector<std::string> plugins;
    DIR *dir = opendir(dir_path.c_str());
    if (!dir) {
        std::cerr << "неверный путь к плагинам: " << dir_path << std::endl;
        return plugins;
    }

    struct dirent *entry; //запись
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name; //имя файла
        if (filename.find(".so") != std::string::npos) { // ищем .so
            plugins.push_back(dir_path + "/" + filename);
        }
    }

    closedir(dir);
    return plugins; //возвращает список путей к плагинам
}

//функция загрузки плагинов, возвращает пару: строка(название) - указатель на фабричную функцию
std::vector<std::pair<std::string, Create_plugin_t>> load_plugins(const std::vector<std::string> &plugin_paths) {
    std::vector<std::pair<std::string, Create_plugin_t>> plugins;
    
    //для каждого пути происходит попытка открыть библиотеку
    for (const auto &path : plugin_paths) {
        // Загрузка библиотеки
        void *handle = dlopen(path.c_str(), RTLD_NOW);
        if (!handle) {
            std::cerr << "ошибка загрузки плагина: " << path << " (" << dlerror() << ")" << std::endl;
            continue;
        }

        // и загрузить фабричную функцию для валидации плагина
        Create_plugin_t create = (Create_plugin_t)dlsym(handle, "Create_plugin");
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "не удалось загрузить фабричную функцию для " << path << " " << dlsym_error << std::endl;
            dlclose(handle);
            continue;
        }

        // в случае успеха плагин добавляется в список
        plugins.emplace_back(path, create);
    }

    return plugins;
}

//выбор и использование плагина
//на выход поступает список валидных плагинов в формате: название - указатель на фабричную функцию создания объекта плагина 
void select_and_run_plugin(const std::vector<std::pair<std::string, Create_plugin_t>> &plugins) {
    if (plugins.empty()) {
        std::cerr << "нет доступных плагинов" << std::endl;
        return;
    }

    // выводим список доступных плагинов
    std::cout << "доступные плагины:" << std::endl;
    for (size_t i = 0; i < plugins.size(); ++i) {
        std::cout << i + 1 << ": " << plugins[i].first << std::endl;
    }

    // выбор плагина
    size_t choice;
    std::cout << "выберите плагин: ";
    std::cin >> choice;

    if (choice < 1 || choice > plugins.size()) {
        std::cerr << "некорректный номер плагина" << std::endl;
        return;
    }

    // структурированное связывание (разложение пары на 2 переменные)
    const auto &[path, create] = plugins[choice - 1];

    // Создаем объект плагина
    BasePlugin *plugin = create();
    plugin->connect();
    plugin->read();
    plugin->write();

    // уничтожаем объект плагина с помощью фаьбричной функции
    Destroy_plugin_t destroy = (Destroy_plugin_t)dlsym(dlopen(path.c_str(), RTLD_NOW), "Destroy_plugin");
    if (destroy) {
        destroy(plugin);
    }
}

int main() {
    // Сканируем директорию с плагинами
    std::vector<std::string> plugin_paths = scan_plugins_directory("plugins");

    // Загружаем плагины
    std::vector<std::pair<std::string, Create_plugin_t>> plugins = load_plugins(plugin_paths);

    // Выбираем и запускаем плагин
    select_and_run_plugin(plugins);

    return 0;
}

/*Команды для компиляции:
основная программа: g++ src/main.cpp -o plugin_example -ldl -Llib -lbaseplugin
плагины: g++ -fPIC -shared -o plugins/libpluginMB.so src/pluginMB.cpp -Llib -lbaseplugin
библиотека: g++ -c src/plugin.cpp -o src/plugin.o -fPIC
            ar rcs lib/libbaseplugin.a src/plugin.o 
*/