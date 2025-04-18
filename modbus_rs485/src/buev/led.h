// #include "data.h"
#include <cstdint>

// LED (BUEV_G-20)
#define BUEV_G20_LED_Gen_R 0x00000040   // Светодиод ГЕНЕРАТОР ВОЗБУД (желтый)
#define BUEV_G20_LED_Gen_G 0x00000020   // Светодиод ГЕНЕРАТОР РЧ (зеленый)
#define BUEV_G20_LED_RPN1_G 0x00000080  // РПН1 зеленый
#define BUEV_G20_LED_RPN1_R 0x00000004  // РПН1 красный
#define BUEV_G20_LED_RPN2_G 0x00000008  // РПН2 зеленый
#define BUEV_G20_LED_RPN2_R 0x00000010  // РПН2 красный
#define BUEV_G20_LED_WORK_G 0x00000001  // светодиод РАБОТА/АВАРИЯ зеленый
#define BUEV_G20_LED_ALARM_R 0x00000002 // светодиод РАБОТА/АВАРИЯ красный
#define BUEV_G20_LED_RPNM_G 0x00010000  // РПНМ зеленый
#define BUEV_G20_LED_RPNM_R 0x00020000  // РПНМ красный
// LED (BUEV-G-10)
#define BUEV_G10_LED_Gen_R 0x00000080   // Светодиод ГЕНЕРАТОР ВОЗБУД (желтый)
#define BUEV_G10_LED_Gen_G 0x00000004   // Светодиод ГЕНЕРАТОР РЧ (зеленый)
#define BUEV_G10_LED_RPN1_G 0x00000008  // РПН1 зеленый
#define BUEV_G10_LED_RPN1_R 0x00000010  // РПН1 красный
#define BUEV_G10_LED_RPN2_G 0x00000020  // РПН2 зеленый
#define BUEV_G10_LED_RPN2_R 0x00000040  // РПН2 красный
#define BUEV_G10_LED_WORK_G 0x00000001  // светодиод РАБОТА/АВАРИЯ зеленый
#define BUEV_G10_LED_ALARM_R 0x00000002 // светодиод РАБОТА/АВАРИЯ красный
#define BUEV_G10_LED_RPNM_G 0x00010000  // РПНМ зеленый
#define BUEV_G10_LED_RPNM_R 0x00020000  // РПНМ красный

class Led {
  public:
    uint32_t LED_Gen_R = BUEV_G10_LED_Gen_R;
    uint32_t LED_Gen_G = BUEV_G10_LED_Gen_G;
    uint32_t LED_RPN1_G = BUEV_G10_LED_RPN1_G;
    uint32_t LED_RPN1_R = BUEV_G10_LED_RPN1_R;
    uint32_t LED_RPN2_G = BUEV_G10_LED_RPN2_G;
    uint32_t LED_RPN2_R = BUEV_G10_LED_RPN2_R;
    uint32_t LED_WORK_G = BUEV_G10_LED_WORK_G;
    uint32_t LED_ALARM_R = BUEV_G10_LED_ALARM_R;
    uint32_t LED_RPNM_G = BUEV_G10_LED_RPNM_G;
    uint32_t LED_RPNM_R = BUEV_G10_LED_RPNM_R;
    

};

extern Led led;