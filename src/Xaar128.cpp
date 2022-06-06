//--------------------LIBRARY INCLUDES--------------------//
#include "Nozzle.h"
#include <Arduino.h>
#include <SPI.h>
#include "Pins.h"

#ifdef XAAR128

uint64_t nozzleData[2] = {0};
uint16_t heaterGoal = 0;

//--------------------FUNCTIONS--------------------//
void initNozzle(){
  pinMode(NOZZLE_NSS1_PIN, OUTPUT);
  digitalWrite(NOZZLE_NSS1_PIN, HIGH);
  pinMode(NOZZLE_NSS2_PIN, OUTPUT);
  digitalWrite(NOZZLE_NSS2_PIN, HIGH);
  pinMode(NOZZLE_VHCHL_PIN, OUTPUT);
  digitalWrite(NOZZLE_VHCHL_PIN, HIGH);
  pinMode(NOZZLE_VDD_PIN, OUTPUT);
  digitalWrite(NOZZLE_VDD_PIN, HIGH);
  pinMode(NOZZLE_FIRE_PIN, OUTPUT);
  digitalWrite(NOZZLE_FIRE_PIN, HIGH);
  pinMode(NOZZLE_RESET_PIN, OUTPUT);
  digitalWrite(NOZZLE_RESET_PIN, LOW); 
  pinMode(NOZZLE_PHO_PIN, OUTPUT);
  digitalWrite(NOZZLE_PHO_PIN, LOW);
  pinMode(HEATER_SWEEP_PIN, OUTPUT);
  digitalWrite(HEATER_SWEEP_PIN, LOW);


  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);

  
  pinMode(NOZZLE_CLK_PIN, OUTPUT);
  TCCR1A = _BV(COM1A0);              // toggle OC1A on compare match
  OCR1A = 7;                         // top value for counter
  TCCR1B = _BV(WGM12) | _BV(CS10);   // CTC mode, prescaler clock/1
}


void setNozzleData(uint64_t* inputArray)
{
  for(uint8_t index = 0; index < 2; index++)
  {
    nozzleData[index] = inputArray[index];
  } 
}


//Turns on the print head
void turnOnNozzle()
{
  digitalWrite(NOZZLE_VDD_PIN, LOW);
  delay(50);
  digitalWrite(NOZZLE_VHCHL_PIN, LOW);
  delay(50);
  digitalWrite(NOZZLE_RESET_PIN, HIGH);
  delay(10);
}



//Turns off the print head
void turnOffNozzle()
{
  digitalWrite(NOZZLE_RESET_PIN, LOW);
  delay(50);
  
  digitalWrite(NOZZLE_VHCHL_PIN, HIGH);
  delay(50);

  digitalWrite(NOZZLE_VDD_PIN, HIGH); 
}



//Writes data over spi to the print head
void loadNozzle()
{
  uint8_t transmissionBuffer[16];

  for(uint8_t currentByte = 0; currentByte < 16; currentByte++)
  {
    if(currentByte < 8){
      transmissionBuffer[currentByte] = nozzleData[0] >> (currentByte * 8) & 0xFF;
    }else{
      transmissionBuffer[currentByte] = nozzleData[1] >> ((currentByte - 8) * 8) & 0xFF;
    }    
  }

  digitalWrite (NOZZLE_NSS1_PIN, HIGH);
  digitalWrite (NOZZLE_NSS2_PIN, LOW);
  
  for(uint8_t currentByte = 0; currentByte < 16; currentByte++)
  { 
    if(currentByte == 8)
    {
      digitalWrite (NOZZLE_NSS1_PIN, LOW);
      digitalWrite (NOZZLE_NSS2_PIN, HIGH);
    }

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(transmissionBuffer[currentByte]);
    SPI.endTransaction();
  }
  
  digitalWrite (NOZZLE_NSS1_PIN, HIGH);
  digitalWrite (NOZZLE_NSS2_PIN, HIGH);
}



//Triggers the print head to fire the data stored inside
void fireNozzle()
{ 
  delayMicroseconds(50);
  digitalWrite(NOZZLE_FIRE_PIN, LOW);
  delayMicroseconds(100);
  digitalWrite(NOZZLE_FIRE_PIN, HIGH);
  delayMicroseconds(1);
}



//Fires a single nozzle from the head for single nozzle printing
void fireSingleNozzle(){
  uint64_t nozzleHolder[2];

  for(uint8_t nozzleSet = 0; nozzleSet < 2; nozzleSet++)
  {
    nozzleHolder[nozzleSet] = nozzleData[nozzleSet];
  }

  nozzleData[1] = 0;
  nozzleData[0] = 1 << 62;

  loadNozzle();
  fireNozzle();

  for(uint8_t nozzleSet = 0; nozzleSet < 2; nozzleSet++)
  {
    nozzleData[nozzleSet] = nozzleHolder[nozzleSet];
  }
}



double getHeatertempp()
{
  return analogRead(HEATER_SWEEP_SENSOR_PIN) * 0.05;
}



void setHeaterGoal(uint8_t newHeaterGoal)
{
  heaterGoal = newHeaterGoal;
}



void updateHeater()
{
  float heatData = analogRead(HEATER_SWEEP_SENSOR_PIN);
  heatData *= 0.05;

  if(heatData < heaterGoal)
  {
    digitalWrite(HEATER_SWEEP_PIN, 1);
  }
  else
  {
    digitalWrite(HEATER_SWEEP_PIN, 0);
  }
}
#endif
