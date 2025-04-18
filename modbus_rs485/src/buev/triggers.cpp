#include "triggers.h"
#include "../utils.h"
#include "led.h"

Triggers trig;

void Triggers::check_trigger(trigger_t *trig, Data::buev_data_t bd, float bdval, float val, int green_val, int red_val, int low) {
    switch (trig->state) {
    case TRIG_STATE_INIT: // Исходное состояние
        if (low) {        // Проверяем, если параметр ниже порога
            if (green_val && bdval < val) {
                trig->state = TRIG_STATE_START; // Переходим в состояние START
                if (trig->dt != 0.0) {
                    trig->fval = bdval; // Сохраняем начальное значение
                }
                gettimeofday(&(trig->started_at), NULL); // Запоминаем время начала
            }
        } else { // Иначе проверяем, если параметр выше порога
            if (green_val && bdval > val) {
                trig->state = TRIG_STATE_START;
                if (trig->dt != 0.0) {
                    trig->fval = bdval;
                }
                gettimeofday(&(trig->started_at), NULL);
            }
        }
        break;

    case TRIG_STATE_START: // Состояние START
        if (low) {         // Если параметр вернулся в норму (выше порога)
            if (bdval >= val) {
                trig->state = TRIG_STATE_FINISH;         // Переходим в состояние FINISH
                gettimeofday(&(trig->started_at), NULL); // Запоминаем время окончания
            }
        } else { // Если параметр вернулся в норму (ниже порога)
            if (bdval <= val) {
                trig->state = TRIG_STATE_FINISH;
                gettimeofday(&(trig->started_at), NULL);
            }
        }
        break;

    case TRIG_STATE_FINISH: // Состояние FINISH
        if (low) {          // Если параметр снова выходит за пределы нормы (ниже порога)
            if (green_val && bdval < val) {
                gettimeofday(&(trig->started_at), NULL); // Запоминаем новое время начала
            }
        } else { // Если параметр снова выходит за пределы нормы (выше порога)
            if (green_val && bdval > val) {
                gettimeofday(&(trig->started_at), NULL);
            }
        }
        trig->dt = ((float)util.timediff_ms_from_now(&(trig->started_at))) / 1000.0; // Вычисляем продолжительность события
        if (red_val) {                                                          // Если установлен красный флаг
            trig->state = TRIG_STATE_INIT;                                      // Возвращаемся в исходное состояние
            trig->sval = bdval;                                                 // Сохраняем новое начальное значение
        }
        break;
    }
}

void Triggers::init_triggers() {
    memset(&trig_rpn1, 0, sizeof(trigger_t));
    memset(&trig_rpn2, 0, sizeof(trigger_t));
    memset(&trig_rpnm, 0, sizeof(trigger_t));
    memset(&trig_wa155, 0, sizeof(trigger_t));
    memset(&trig_rmtz, 0, sizeof(trigger_t));
    memset(&trig_phfail, 0, sizeof(trigger_t));
    memset(&trig_rmtr, 0, sizeof(trigger_t));
}

void Triggers::check_triggers(Data::buev_data_t bd) {
    // Обновляем триггер РПН1
    check_trigger(&trig_rpn1, bd, bd.Vacc, bd.Vrpn1, bd.led & led.LED_RPN1_G, bd.led & led.LED_RPN1_R, 0);

    // Обновляем триггер РПН2
    check_trigger(&trig_rpn2, bd, bd.Vacc, bd.Vrpn2, bd.led & led.LED_RPN2_G, bd.led & led.LED_RPN2_R, 0);

    // Обновляем триггер РПНМ
    check_trigger(&trig_rpnm, bd, bd.V100V, bd.Vrpnm, bd.led & led.LED_RPNM_G, bd.led & led.LED_RPNM_R, 0);

    // Обновляем триггер напряжения АКБ > 155В
    check_trigger(&trig_wa155, bd, bd.Vacc, TRIG_WA155_VAL, bd.led & led.LED_WORK_G, bd.led & led.LED_ALARM_R, 1);

    // Обновляем триггер максимального тока зарядки АКБ > 100А
    check_trigger(&trig_rmtz, bd, bd.Acharge, TRIG_RMTZ_VAL, bd.led & led.LED_WORK_G, bd.led & led.LED_ALARM_R, 1);

    // Обновляем триггер перекоса фаз > 40%
    check_trigger(&trig_phfail, bd, bd.perekos, TRIG_PHFAIL_VAL, !(bd.alarm & ALARM_ObrFaz), bd.alarm & ALARM_ObrFaz, 1);

    // Обновляем триггер максимального тока разрядки АКБ < -180А
    check_trigger(&trig_rmtr, bd, bd.Acharge, TRIG_RMTR_VAL, !(bd.status & STATUS_RMTR), bd.status & STATUS_RMTR, 0);
}