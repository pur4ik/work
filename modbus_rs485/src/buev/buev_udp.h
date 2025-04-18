/*
сходить отладиться на пси с учетом блока 20ки:
    -рпн2 почему-то серый не работает триггер (мб из-за блока 20)
    -рч - скорость больше 35-36 зеленый
        сбавляешь скорость - тухнет
    -ген не горит когда генератор не включен
        желтый когда низкая скорость до 30+/-
        дальнейший разгон срабатывает рч и ген - зелёный
        обрыв фаз скорее не работает корректно по времени
*/

#ifndef BUEV_UDP_H
#define BUEV_UDP_H

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "data.h"

class Buev_udp {

  private:
    static constexpr const char *BUEV_IP_ADDR = "10.7.8.101";
    static constexpr int BUEV_UDP_PORT = 19003;
    static constexpr size_t BUFFER_SIZE = 4096; // Размер буфера для данных
    int udp_sock_;                              // Дескриптор UDP-сокета
    struct sockaddr_in udp_addr_;               // Адрес для привязки UDP-сокета
    uint8_t udp_buffer_[BUFFER_SIZE];           // Буфер для хранения данных
    fd_set rfds_;
    struct timeval tv_; // Таймаут для select

  public:
    bool bm_connection = false;
    bool bm_led_connection = false;


    ~Buev_udp() { closeSocket(); }

    void init ();
    bool readData(uint8_t *buffer, size_t buffer_size, size_t &bytes_read);
    void closeSocket();

};

extern Buev_udp buevg_udp;
#endif