#ifndef LASER_H
#define LASER_H

#include "stdint.h"

void setNozzlePower(uint8_t newLaserPower);
void turnOffNozzle();
void turnOnNozzle();
void initNozzle();

#endif
