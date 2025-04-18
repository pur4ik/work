#include <iostream>
#include <cstring>
#include "visa.h"
#include "mm.h"

int main() {    
    float napr = akip.read_u ();
    visa_device.visa_write ("SYST:LOC");
    std::cout << napr << "\n";
    return 0;
}

