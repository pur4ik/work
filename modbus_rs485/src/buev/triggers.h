
 //#include "buev_udp.h"
 #include "data.h"

#define STATUS_RASWAL 0x0004            // 1 - развал схемы
#define STATUS_RPN1 0x0008              // 1 - реле пониженного напряжения РПН1 откл
#define STATUS_RPN2 0x0010              // 1 - реле пониженного напряжения РПН2 откл
#define STATUS_RMN 0x0020               // 1 - реле максимального напряжения РМН вкл
#define STATUS_RPNM 0x0040              // 1 - реле пониженного напряжения РПНМ откл
#define STATUS_KEY 0x0080               // Состояние ключа (открыт/закрыт)
#define STATUS_KEYA 0x0100              // ХЗ. не испульзуется судя по коду
#define STATUS_RCH 0x0200               // 1 - реле частоты вкл
#define STATUS_RN100 0x0400             // 1 - реле напряжения 100В  РН100 вкл
#define STATUS_ROW 0x0800               // 1 - реле обмотки возбуждения РОВ вкл
#define STATUS_ButtonA 0x1000           // кнопка Развал защиты (Авария)
#define STATUS_ButtonW 0x2000           // кнопка Восстановление защиты
#define STATUS_Cik 0x4000               // 1 - пусковое реле вкл
#define STATUS_RMTR 0x8000              // 1 - сигнал максимального тока разряда АКБ РМТ-Р
#define STATUS_GEN_PERMITION 0x00010000 // 1 - управление генератором разрешено
#define STATUS_DETECT_MIN_SPEED 0x00020000
#define STATUS_GEN_EXITATION 0x00040000
#define STATUS_WORK_MODE 0x00300000
#define STATUS_TEMP_DENIED 0x00400000

// DIP
#define DIP_ACC 0x03 // тип АКБ:
#define DIP_KW 0x0C  // тип генератора:

// Alarm
#define ALARM_RASWAL 0x01    // Защита развалена
#define ALARM_PerFaz 0x02    // Перекос фаз
#define ALARM_ObrFaz 0x04    // Обрыв фазы
#define ALARM_AmpAkk120 0x08 // Ток заряда АКБ больше 120А
#define ALARM_NapAkk250 0x10 // Напряжение АКБ больше 250В
#define ALARM_NapAkk165 0x20 // Напряжение АКБ больше 165В
#define ALARM_NapAkk155 0x40 // Напряжение АКБ больше 155В
#define ALARM_TempAkk 0x80   // Датчик температуры неисправен

// DIP5 OFF
#define TRIG_RPN1_VAL_56PzS_350P_0 99.3
#define TRIG_RPN1_VAL_90KL_300P_0 99.3
#define TRIG_RPN1_VAL_56PzV_385P_0 99.3
#define TRIG_RPN1_VAL_90KGL_300P_0 99.3

#define TRIG_RPN2_VAL_56PzS_350P_0 95.7
#define TRIG_RPN2_VAL_90KL_300P_0 94.3
#define TRIG_RPN2_VAL_56PzV_385P_0 95.7
#define TRIG_RPN2_VAL_90KGL_300P_0 94.3

#define TRIG_RPNM_VAL_56PzS_350P_0 95.7
#define TRIG_RPNM_VAL_90KL_300P_0 94.3
#define TRIG_RPNM_VAL_56PzV_385P_0 95.7
#define TRIG_RPNM_VAL_90KGL_300P_0 94.3

// DIP5 ON
#define TRIG_RPN1_VAL_56PzS_350P_1 105.0
#define TRIG_RPN1_VAL_90KL_300P_1 105.0
#define TRIG_RPN1_VAL_56PzV_385P_1 105.0
#define TRIG_RPN1_VAL_90KGL_300P_1 105.0

#define TRIG_RPN2_VAL_56PzS_350P_1 100.0
#define TRIG_RPN2_VAL_90KL_300P_1 100.0
#define TRIG_RPN2_VAL_56PzV_385P_1 100.0
#define TRIG_RPN2_VAL_90KGL_300P_1 100.0

#define TRIG_RPNM_VAL_56PzS_350P_1 100.0
#define TRIG_RPNM_VAL_90KL_300P_1 100.0
#define TRIG_RPNM_VAL_56PzV_385P_1 100.0
#define TRIG_RPNM_VAL_90KGL_300P_1 100.0

#define TRIG_WA155_VAL 155.0
#define TRIG_RMTZ_VAL 100.0
#define TRIG_RMTR_VAL -180.0
#define TRIG_PHFAIL_VAL 40.0

class Triggers {
  private:
  public:
    typedef enum { TRIG_STATE_INIT = 0, TRIG_STATE_START, TRIG_STATE_FINISH, TRIG_STATE_MAX } trigger_state_t;

    typedef struct {
        float dt;                  // Время срабатывания триггера (секунды)
        float sval;                // Начальное значение параметра при срабатывании
        float fval;                // Конечное значение параметра при возвращении в норму
        struct timeval started_at; // Время начала срабатывания триггера
        trigger_state_t state;     // Текущее состояние триггера
    } trigger_t;

    trigger_t trig_rpn1;
    trigger_t trig_rpn2;
    trigger_t trig_rpnm;
    trigger_t trig_wa155;
    trigger_t trig_rmtz;
    trigger_t trig_phfail;
    trigger_t trig_rmtr;

    void check_trigger(trigger_t *trig, Data::buev_data_t bd, float bdval, float val, int green_val, int red_val, int low);
    void check_triggers(Data::buev_data_t bd);
    void init_triggers();
};

extern Triggers trig;