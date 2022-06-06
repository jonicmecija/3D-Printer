#include "Laser.h"
#include "Arduino.h"
#include "Pins.h"
#include "Settings.h"

#ifdef TTL_LASER

uint8_t laserPower = 0;

void setNozzlePower(uint8_t newLaserPower){
  laserPower = newLaserPower;
}

void initNozzle()
{
  pinMode(LASER_PIN, OUTPUT);
}

void turnOffNozzle()
{
  analogWrite(LASER_PIN, laserPower);
}

void turnOnNozzle()
{
  analogWrite(LASER_PIN, 0);
}
#endif
