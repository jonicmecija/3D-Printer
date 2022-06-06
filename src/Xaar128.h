#ifndef XAAR128_H
#define XAAR128_H

#include <stdint.h>

void turnOnNozzle();
void turnOffNozzle();
void initNozzle();
void loadNozzle();
void fireNozzle();
void fireSingleNozzle();
void setNozzleData(uint64_t* inputArray);
void setHeaterGoal(uint8_t newHeaterGoal);
double getHeatertempp();
void updateHeater();

#endif
