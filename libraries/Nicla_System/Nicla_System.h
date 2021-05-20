#ifndef Nicla_System_h_
#define Nicla_System_h_

#include "Arduino.h"
#include "BQ25120A.h"
#include "IS31FL3194.h"

void RgbOn(uint8_t color);
void RgbOff();
bool enable3V3LDO();
bool enable1V8LDO();
bool disableLDO();

#endif