#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <regex>

class Buev_uart {
   private:
    int fd;  // дескриптор порта
    std::string port_name;
    const std::string seven_str = "ZP";
    const std::string nine_str = "FW";
    const std::string zero_str = "P:(";
    const std::string test_str = "a0";
    const std::string temp_str = "TEMP";
    const std::string start_str = "START";

    bool zero = false;
    bool seven = false;
    bool nine = false;
    bool test = false;
    bool start = false;

    size_t pos;
    int counter;
    int mode;

    std::string constant_string;
    std::string inconstant_string;

    std::string number;
    std::vector<int> numbers;
    std::chrono::steady_clock::time_point lastUpdateTime;  // Время последнего обновления

   public:
    bool time = false;
    bool temp = false;
    std::string buev_log;
    struct ParsedData {
        bool start = false;
        int time;  // время с момента зауска ядра
        int fwv;   // напряжение обмотки возбуждения 2) [100-1вольт]
        int rsv;   // требуемое напряжение уставки 3)  [вольт]
        int spf;   // коэф накачки 0-выкл 1-авторежим ... 1000 - 100% 4)
        int iov;   // ток обмотки возбуждения [2537 = 2.537 АМПЕР] 5)
        int ur;    // напряжение выпрямления [2000 = 20 ВОЛЬТ] 6)
        int up;    // напряжение накачки [2000 = 20вольт] 7)
        int uo;    // напряжение обмотки возбуждения [2000 = 20 вольт]
        int cp;    // параметр компенсации
        int u24;   // напряжение блока  [2000 = 20 вольт]
        /// далее параметры crc
        int f;          // частота [1500 = 150гц]
        int r;          // реле [1-вкл 0-выкл] (вкл при ur > 4000 && u24 > 2000)
        int pm;         // перекос фаз [10 < - ок] [> 10 - перекос]
        int pt;         // теоретически степень открытия ключа обмотки возбуждения [0 - 0%
                        // 65534 - 100%]
        int pr;         // реальное открытие ключа обмотки возбуждения [0 - 0% 65534 - 100%]
        int div;        // разница между реальным и теоретическим о.к.о.в. [-65534 - 65534]
        int spi;        // проверка режима работы spi [0-холодный пуск || 10000 - плата
                        // управления защиты вкл] (определяет источник команд)
        std::string wm; /*режим работы :00-//ФАЗА холодного пуска
                                01-//ФАЗА холодного пуска
                                10-старт//ФАЗА холодного пуска
                                11-выставление переменных//ФАЗА холодного пуска
                                12-накачка//ФАЗА холодного пуска
                                13-ожидание тока//ФАЗА холодного пуска
                                14-выполнение процедуры холодного пуска//ФАЗА
                   холодного пуска

                                20-выставление параметров    //ВЫХОД из режима
                   холодного пуска 21-выставление параметров 2  //ВЫХОД из режима
                   холодного пуска 22-основной рабочий режим //ВЫХОД из режима
                   холодного пуска */
        int test;       //???
        int ig;         // ток генератора [неотрицательный 0-230] (если аномально большой,
                        // значит пришло отрицательное значение)
        int iab;        // ток акк. батареи  [-200 - 200]
        int uab;        // напряжение акк батареи [134000 = 134вольта]
        int fw;         // версия прошивки если отсутствует - ПРОШИВКА УСТАРЕВШАЯ!!!

        // группа доп парамтеров параметров
        int zp;      // нулевая точка АЦП тока (26700 - флешка не прочитана)
        int corph1;  // корректировка фаз
        int corph2;
        int corph3;
        int pha;         //
        int phb;         //
        int phc;         //
        int temp_value;  // температура
    };

    Buev_uart(const std::string &port, int baudrate) : fd(-1), port_name(port) {
        // Открытие порта
        fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            //throw std::runtime_error("Не удалось открыть порт: " + port);
        }
        fcntl(fd, F_SETFL, 0);  // Установка в блокирующий режим

        // Настройка порта
        struct termios options;
        if (tcgetattr(fd, &options) != 0) {
            close(fd);
            //throw std::runtime_error("Не удалось получить атрибуты порта");
        }

        // Устанавливаем скорость
        speed_t speed = baudrate;

        cfsetispeed(&options, speed);
        cfsetospeed(&options, speed);

        // Настройка: 8N1
        options.c_cflag &= ~PARENB;  // Отключить четность
        options.c_cflag &= ~CSTOPB;  // 1 стоп-бит
        options.c_cflag &= ~CSIZE;   // Очистить битовые маски размера
        options.c_cflag |= CS8;      // Установить 8 бит данных

        options.c_cflag |= (CLOCAL | CREAD);  // Включить чтение и игнорировать управление модемом

        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Отключить канонический ввод, эхо и сигналы
        options.c_iflag &= ~(IXON | IXOFF | IXANY);          // Отключить управление потоком
        options.c_oflag &= ~OPOST;                           // Отключить постобработку вывода

        // Устанавливаем конфигурацию порта
        if (tcsetattr(fd, TCSANOW, &options) != 0) {
            close(fd);
            //throw std::runtime_error("Не удалось установить атрибуты порта");
        }

        std::cout << "Порт " << port << " успешно открыт и настроен." << std::endl;
    }

    ~Buev_uart() {
        if (fd != -1) {
            close(fd);
            std::cout << "Порт " << port_name << " закрыт." << std::endl;
        }
    }

    ParsedData data;

    void updateTimer(); 

    void start_parse(const std::string &str);
    void finish_parse(std::string &constant_string, int mode);
    void separator(std::string &incostant_string, int count);
    ParsedData parse(const std::string &str, bool zero, bool seven, bool nine, bool test);
    int write_data(const std::string &data);
    std::string read_data(size_t size);
};

extern Buev_uart uart;