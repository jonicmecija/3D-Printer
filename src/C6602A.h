#ifndef C6602A_H
#define C6602A_H

#include <stdint.h>

void turnOnNozzle();
void turnOffNozzle();
void initNozzle();
void loadNozzle();
void fireNozzle();
void fireSingleNozzle();
void setNozzleData(uint64_t* inputArray);
void setHeaterGoal(uint8_t newHeaterGoal);
float getHeatertempp();
void updateHeater();

#endif
