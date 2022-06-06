#ifndef FDMEXTRUDER_H
#define FDMEXTRUDER_H

#include "stdint.h"

void turnOnNozzle();
void turnOffNozzle();
void initNozzle();
void updateExtruderMotor();
void moveExtruder(int32_t position);
void setExtruderAbsolute();
void setExtruderRelative();
void setExtruderPosition(int32_t Position);
void setHeaterFanSpeed(uint8_t newFanSpeed);
uint8_t getHeaterFanSpeed();
void setFilamentFanSpeed(uint8_t newFanSpeed);
uint8_t getFilamentFanSpeed();
void setHeaterGoal(uint8_t newHeaterGoal);
double getHeatertempp();
void updateHeater();
void setBedGoal(uint8_t newBedGoal);
double getBedTemp();
void updateBed();


#endif
