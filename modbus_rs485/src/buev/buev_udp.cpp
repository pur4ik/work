#include "buev_udp.h"

Buev_udp buevg_udp;

void Buev_udp::init() {
    // Создание UDP-сокета
    udp_sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock_ < 0) {
        std::cerr << "ERROR: Can't open UDP socket!" << std::endl;
        return;
    }

    // Установка неблокирующего режима
    int flags = fcntl(udp_sock_, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "ERROR: Can't get socket flags!" << std::endl;
        closeSocket();
        return;
    }
    if (fcntl(udp_sock_, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "ERROR: Can't set non-blocking mode!" << std::endl;
        closeSocket();
        return;
    }

    // Настройка адреса для привязки сокета
    memset(&udp_addr_, 0, sizeof(udp_addr_));
    udp_addr_.sin_family = AF_INET;
    udp_addr_.sin_addr.s_addr = htonl(INADDR_ANY);  // Принимаем данные на все интерфейсы
    udp_addr_.sin_port = htons(BUEV_UDP_PORT);

    // Привязка сокета к порту
    if (bind(udp_sock_, reinterpret_cast<struct sockaddr *>(&udp_addr_), sizeof(udp_addr_)) == -1) {
        std::cerr << "ERROR: Can't bind UDP socket to port " << BUEV_UDP_PORT << "!" << std::endl;
        closeSocket();
        return;
    }

    std::cout << "UDP socket successfully bound to port " << BUEV_UDP_PORT << "." << std::endl;
    //bm_connection = true;
}

bool Buev_udp::readData(uint8_t *buffer, size_t buffer_size, size_t &bytes_read) {
    FD_ZERO(&rfds_);
    FD_SET(udp_sock_, &rfds_);

    // Устанавливаем таймаут в 1 секунду
    tv_.tv_sec = 1;
    tv_.tv_usec = 0;

    // Ждем данные с использованием select
    int res = select(udp_sock_ + 1, &rfds_, nullptr, nullptr, &tv_);
    if (res < 0) {
        std::cerr << "ERROR: select() failed!" << std::endl;
    } else if (res == 0) {
        bm_connection = false;
        bm_led_connection = false;
        return false;
    }

    if (res == 0) {
        std::cerr << "Timeout: No data received!" << std::endl;
        bm_connection = false;
        bm_led_connection = false;
        return false;
    }

    // Пытаемся прочитать данные
    bytes_read = recvfrom(udp_sock_, buffer, buffer_size, 0, nullptr, nullptr);
    if (bytes_read < 0) {
        std::cerr << "ERROR: Failed to receive data from UDP socket!" << std::endl;
        bm_connection = false;
        bm_led_connection = false;
        return false;
    }

    if (data_obj.parsePacket(buffer, bytes_read)) {
        bm_led_connection = true;
        return true;
    } else {
        bm_led_connection = false;
        bm_connection = false;
        return false;
    }
}

// это для утилс

// void Buev_udp::check_trigger(trigger_t *trig, buev_data_t *bd, float bdval, float val, int green_val, int red_val, int low) {
//     switch (trig->state) {
//     case TRIG_STATE_INIT: // Исходное состояние
//         if (low) {        // Проверяем, если параметр ниже порога
//             if (green_val && bdval < val) {
//                 trig->state = TRIG_STATE_START; // Переходим в состояние START
//                 if (trig->dt != 0.0) {
//                     trig->fval = bdval; // Сохраняем начальное значение
//                 }
//                 gettimeofday(&(trig->started_at), NULL); // Запоминаем время начала
//             }
//         } else { // Иначе проверяем, если параметр выше порога
//             if (green_val && bdval > val) {
//                 trig->state = TRIG_STATE_START;
//                 if (trig->dt != 0.0) {
//                     trig->fval = bdval;
//                 }
//                 gettimeofday(&(trig->started_at), NULL);
//             }
//         }
//         break;

//     case TRIG_STATE_START: // Состояние START
//         if (low) {         // Если параметр вернулся в норму (выше порога)
//             if (bdval >= val) {
//                 trig->state = TRIG_STATE_FINISH;         // Переходим в состояние FINISH
//                 gettimeofday(&(trig->started_at), NULL); // Запоминаем время окончания
//             }
//         } else { // Если параметр вернулся в норму (ниже порога)
//             if (bdval <= val) {
//                 trig->state = TRIG_STATE_FINISH;
//                 gettimeofday(&(trig->started_at), NULL);
//             }
//         }
//         break;

//     case TRIG_STATE_FINISH: // Состояние FINISH
//         if (low) {          // Если параметр снова выходит за пределы нормы (ниже порога)
//             if (green_val && bdval < val) {
//                 gettimeofday(&(trig->started_at), NULL); // Запоминаем новое время начала
//             }
//         } else { // Если параметр снова выходит за пределы нормы (выше порога)
//             if (green_val && bdval > val) {
//                 gettimeofday(&(trig->started_at), NULL);
//             }
//         }
//         trig->dt = ((float)timediff_ms_from_now(&(trig->started_at))) / 1000.0; // Вычисляем продолжительность события
//         if (red_val) {                                                          // Если установлен красный флаг
//             trig->state = TRIG_STATE_INIT;                                      // Возвращаемся в исходное состояние
//             trig->sval = bdval;                                                 // Сохраняем новое начальное значение
//         }
//         break;
//     }
// }

// void Buev_udp::init_triggers() {
//     memset(&trig_rpn1, 0, sizeof(trigger_t));
//     memset(&trig_rpn2, 0, sizeof(trigger_t));
//     memset(&trig_rpnm, 0, sizeof(trigger_t));
//     memset(&trig_wa155, 0, sizeof(trigger_t));
//     memset(&trig_rmtz, 0, sizeof(trigger_t));
//     memset(&trig_phfail, 0, sizeof(trigger_t));
//     memset(&trig_rmtr, 0, sizeof(trigger_t));
// }

// void Buev_udp::check_triggers(buev_data_t *bd) {
//     // Обновляем триггер РПН1
//     check_trigger(&trig_rpn1, bd, bd->Vacc, bd->Vrpn1, bd->led & LED_RPN1_G, bd->led & LED_RPN1_R, 0);

//     // Обновляем триггер РПН2
//     check_trigger(&trig_rpn2, bd, bd->Vacc, bd->Vrpn2, bd->led & LED_RPN2_G, bd->led & LED_RPN2_R, 0);

//     // Обновляем триггер РПНМ
//     check_trigger(&trig_rpnm, bd, bd->V100V, bd->Vrpnm, bd->led & LED_RPNM_G, bd->led & LED_RPNM_R, 0);

//     // Обновляем триггер напряжения АКБ > 155В
//     check_trigger(&trig_wa155, bd, bd->Vacc, TRIG_WA155_VAL, bd->led & LED_WORK_G, bd->led & LED_ALARM_R, 1);

//     // Обновляем триггер максимального тока зарядки АКБ > 100А
//     check_trigger(&trig_rmtz, bd, bd->Acharge, TRIG_RMTZ_VAL, bd->led & LED_WORK_G, bd->led & LED_ALARM_R, 1);

//     // Обновляем триггер перекоса фаз > 40%
//     check_trigger(&trig_phfail, bd, bd->perekos, TRIG_PHFAIL_VAL, !(bd->alarm & ALARM_ObrFaz), bd->alarm & ALARM_ObrFaz, 1);

//     // Обновляем триггер максимального тока разрядки АКБ < -180А
//     check_trigger(&trig_rmtr, bd, bd->Acharge, TRIG_RMTR_VAL, !(bd->status & STATUS_RMTR), bd->status & STATUS_RMTR, 0);
// }

// мб это не дает триггерам работать
//  void Buev_udp::update_leds(buev_data_t *bd) {
//      // Обновляем состояние светодиода РПН1
//      if (bd->Vacc < bd->Vrpn1) {
//          bd->led |= LED_RPN1_R; // Включаем красный светодиод
//          bd->led &= ~LED_RPN1_G; // Выключаем зеленый светодиод
//      } else {
//          bd->led |= LED_RPN1_G; // Включаем зеленый светодиод
//          bd->led &= ~LED_RPN1_R; // Выключаем красный светодиод
//      }

//     // Обновляем состояние светодиода РПН2
//     if (bd->Vacc < bd->Vrpn2) {
//         bd->led |= LED_RPN2_R;
//         bd->led &= ~LED_RPN2_G;
//     } else {
//         bd->led |= LED_RPN2_G;
//         bd->led &= ~LED_RPN2_R;
//     }

//     // Обновляем состояние светодиода РПНМ
//     if (bd->V100V < bd->Vrpnm) {
//         bd->led |= LED_RPNM_R;
//         bd->led &= ~LED_RPNM_G;
//     } else {
//         bd->led |= LED_RPNM_G;
//         bd->led &= ~LED_RPNM_R;
//     }

//     // Обновляем состояние светодиода РАБОТА/АВАРИЯ
//     if (bd->alarm != 0) {
//         bd->led |= LED_ALARM_R; // Аварийная ситуация — красный светодиод
//         bd->led &= ~LED_WORK_G; // Отключаем зеленый светодиод
//     } else {
//         bd->led |= LED_WORK_G; // Нормальная работа — зеленый светодиод
//         bd->led &= ~LED_ALARM_R; // Отключаем красный светодиод
//     }
// }

void Buev_udp::closeSocket() {
    bm_connection = false;
    if (udp_sock_ >= 0) {
        close(udp_sock_);
        udp_sock_ = -1;  // Обнуляем дескриптор после закрытия
    }
}