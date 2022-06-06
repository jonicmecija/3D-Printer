#include "Arduino.h"
#include "Pins.h"
#include "Settings.h"
#include "Mill.h"


#ifdef MILL

int8_t millPower = 0;

void setNozzlePower(int8_t newNozzlePower){
  millPower = newNozzlePower;
}

void turnOffNozzle()
{
  analogWrite(MILL_PIN, 127);
}


void turnOnNozzle()
{
  analogWrite(MILL_PIN, millPower);
}


void initNozzle()
{
  pinMode(MILL_PIN, OUTPUT);
  turnOffNozzle;
}


#endif
