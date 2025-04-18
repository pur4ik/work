#include "uart.h"
#include <chrono>
#include <thread>
Uart uart;

int Uart::uart_open(const char *port, int baud, int blocking) {
    uartFd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    if (uartFd < 0) {
        fprintf(stderr, "error %d opening %s: %s", errno, port, strerror(errno));
        std::cout << "\n";
        return -1;
    }
    set_interface_attribs(uartFd, baud, 0); // set speed, 8n1 (no parity)
    set_blocking(uartFd, blocking);         // set blocking mode
    printf("Port %s opened.\n", port);
    return 1;
}

int Uart::set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        fprintf(stderr, "error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0;        // no signaling chars, no echo,
                            // no canonical processing
    tty.c_oflag = 0;        // no remapping, no delays
    tty.c_cc[VMIN] = 0;     // read doesn't block
    tty.c_cc[VTIME] = 10;   // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

void Uart::set_blocking(int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        fprintf(stderr, "error %d from tggetattr", errno);
        return;
    }

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        fprintf(stderr, "error %d setting term attributes", errno);
}

int Uart::uart_write(const char *string) {
    std::cout <<"отправил " << string << "\n"; 
    flush_uart ();
    if (write(uartFd, string, strlen(string)) != -1) {
        tcflush(uartFd, TCOFLUSH);
        return 0;
    } else {
        tcflush(uartFd, TCOFLUSH);
        std::cout << "ERROR WRITING" << "\n";
        return -1;
    }
}

std::string Uart::uart_read() {
    std::string responce;
    char buffer[256] = {0};
    responce.clear();
    size_t status = read(uartFd, buffer, sizeof(buffer));
    for (int j = 0; j < status; j++) {
        responce.push_back(buffer[j]);
    }
    memset(buffer, 0, sizeof(buffer));
    status = 0;
    std::cout << "считал " << responce << "\n";
    return responce;
}

void Uart::flush_uart() {
    tcflush(uartFd, TCIFLUSH);
    tcflush(uartFd, TCOFLUSH);
    // Очищает входной буфер UART
}