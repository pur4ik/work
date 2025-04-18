#include "uart.h"
#include "utils.h"
#include "cstring"

class Bp {
  private:
  public:
    ~Bp() { stop(); }


    float read_u ();
    float read_i ();
    int init();
    void stop();
    
    int set_u(float u_value, float time = 0);
    int set_i(float i_value, int time = 0);

    int out_on ();
    int out_off ();
    //вкл
    //выкл
};
extern Bp bp;