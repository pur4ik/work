#include "mm.h"

Mm akip;

float Mm::read_u() {
  visa_device.visa_write(":MEASure:VOLTage:DC?");
  float value = stof(visa_device.visa_read());
  return value;
}
