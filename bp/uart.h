#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

class Uart {
  private:
    const char *port = "/dev/ttyUSB0";
    int baud = 38400;
    int blocking = 0;
    int uartFd = -1;

  public:

    Uart() {
        if (uart_open(port, baud, blocking) != 1) {
            std::cout << "error uart connection" << " \n";
        }
    }

    ~Uart() { close(uartFd); }

    int uart_open(const char *port, int baud, int blocking);
    void uart_close() { close(uartFd); }
    int uart_write(const char *string);
    std::string uart_read();
    int set_interface_attribs(int fd, int speed, int parity);
    void set_blocking(int fd, int should_block);
    void flush_uart();
};

extern Uart uart;