using namespace std;

#include "bp.h"

#include <chrono>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>

int main() {
    bp.init();
    bp.set_u(11.0);
    bp.out_on();
    sleep(5);
    bp.out_off();
    // bp.init();
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // sleep (1);
    // //bp.out_on ();
    // bp.set_u (15.0);
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // //bp.set_u (5.0, 5000);
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // float test2 = bp.read_u();
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // float test = bp.read_i ();
    // cout << test << " " << test2 << "\n";
    // sleep (10);
    // bp.stop();

     return 0;
}
