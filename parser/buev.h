#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <termios.h>
#include <unistd.h>

class Buev {
  private:
    int fd; // дескриптор порта
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
    bool temp = false;
    bool start = false;

    size_t pos;
    int counter;
    int mode;

    std::string constant_string;
    std::string inconstant_string;

    std::string number;
    std::vector<int> numbers;

  public:
    std::string buev_log;
    struct ParsedData {
        bool start = false;
        int time; 
        int fwv;  
        int rsv;  
        int spf;  
        int iov;  
        int ur;   
        int up;   
        int uo;   
        int cp;
        int u24;  
        int f;        
        int r;        
        int pm;       
        int pt;                   
        int pr;                     
        int div;        
        int spi;        
                        
        std::string wm; 
        int test;       
        int ig;         
        int iab;        
        int uab;        
        int fw;         
        int zp;     
        int corph1; 
        int corph2;
        int corph3;
        int pha;  
        int phb;  
        int phc;  
        int temp = 20; 
    };

    Buev(const std::string &port, int baudrate) : fd(-1), port_name(port) {
        fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) {
            throw std::runtime_error("Не удалось открыть порт: " + port);
        }
        fcntl(fd, F_SETFL, 0);

        struct termios options;
        if (tcgetattr(fd, &options) != 0) {
            close(fd);
            throw std::runtime_error("Не удалось получить атрибуты порта");
        }

        speed_t speed = baudrate;

        cfsetispeed(&options, speed);
        cfsetospeed(&options, speed);

        options.c_cflag &= ~PARENB; 
        options.c_cflag &= ~CSTOPB; 
        options.c_cflag &= ~CSIZE;  
        options.c_cflag |= CS8;     
        options.c_cflag |= (CLOCAL | CREAD); 

        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
        options.c_iflag &= ~(IXON | IXOFF | IXANY);         
        options.c_oflag &= ~OPOST;                          

        if (tcsetattr(fd, TCSANOW, &options) != 0) {
            close(fd);
            throw std::runtime_error("Не удалось установить атрибуты порта");
        }

        std::cout << "Порт " << port << " успешно открыт и настроен." << std::endl;
    }

    ~Buev() {
        if (fd != -1) {
            close(fd);
            std::cout << "Порт " << port_name << " закрыт." << std::endl;
        }
    }

    ParsedData data;
    void start_parse(const std::string &str);
    void finish_parse(std::string &constant_string, int mode);
    void separator(std::string &incostant_string, int count);
    ParsedData parse(const std::string &str, bool zero, bool seven, bool nine, bool test);
    void write_data(const std::string &data);
    std::string read_data(size_t size);
};