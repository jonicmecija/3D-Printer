#include "Arduino.h"
#include "USBSource.h"



uint8_t endOfCommandList = 0;


void resetCommandList()
{
  endOfCommandList = 0;
}


void returnOkSignal()
{
    Serial.println("ok");
}


uint8_t isEndOfCommandList()
{
  return endOfCommandList;
}


uint8_t getUSBCharacter()
{
  uint32_t timeoutTime = millis() + 50;
  while(millis() < timeoutTime)
  {
    if(Serial.available()){
    //char read = Serial.read();
    //Serial.println(read);
    //return read;
    return Serial.read();
    }
  }

  endOfCommandList = 1;
  return 0;  
}
