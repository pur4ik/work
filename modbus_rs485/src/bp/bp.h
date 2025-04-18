#ifndef BP_H
#define BP_H

#include "uart.h"
#include "../utils.h"
#include "cstring"
#include "../mm/mm.h"

class Bp {
  private:
  public:
    ~Bp() { 
      std::cout << "DESTR BP \n";
      stop();
     }

    bool power = false;
    float read_u ();
    float read_i ();
    int init();
    void stop();
    
    int set_u(float u_value, float time = 0);
    int set_i(float i_value, int time = 0);

    int set_u_kp (float u_value);

    int out_on ();
    int out_off ();
    //вкл
    //выкл
};
extern Bp bp;
#endif