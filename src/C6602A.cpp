//--------------------LIBRARY INCLUDES--------------------//
#include "Nozzle.h"
#include <Arduino.h>
#include <SPI.h>
#include "Pins.h"

#ifdef C6602A

//--------------------VARIABLES--------------------//
static uint64_t nozzleData[2] = {0};
static uint16_t heaterGoal = 0;
static uint32_t lastRowTime = 0;
static uint32_t calls = 0;

//--------------------FUNCTIONS--------------------//
/*! \brief Initializes pins needed for the print head to
 *         function. Call at beginning of code.
 */
void initNozzle()
{
  lastRowTime = millis();
  pinMode(NOZZLE_SAFTY_PIN, OUTPUT);
  digitalWrite(NOZZLE_SAFTY_PIN, LOW);
  pinMode(NOZZLE_DATA_1_PIN, OUTPUT);
  pinMode(NOZZLE_DATA_2_PIN, OUTPUT);
  pinMode(NOZZLE_DATA_3_PIN, OUTPUT);
  pinMode(NOZZLE_DATA_4_PIN, OUTPUT);
  pinMode(HEATER_SWEEP_PIN, OUTPUT);
  digitalWrite(HEATER_SWEEP_PIN, LOW);
}


/*! \brief Sets the internal nozzle data variable form
 *         an external source
 *         
 *         This function causes no actuation
 */
void setNozzleData(uint64_t* inputArray)
{
  for(uint8_t index = 0; index < 2; index++)
  {
    nozzleData[index] = inputArray[index];
  }
}


/*! \brief Powers on the print head to allow it
 *         to execute any given commands
 */
void turnOnNozzle()
{
  digitalWrite(NOZZLE_SAFTY_PIN, HIGH);
}



/*! \brief Turn off print head, will not respond
 *         to any new commands
 */
void turnOffNozzle()
{
  digitalWrite(NOZZLE_SAFTY_PIN, LOW);
}



/*! \brief Compatability function for the xaar128
 *         print head code. Do not use
 */
void loadNozzle()
{
 
}



/*! \brief Loads and fires all of the nozzles in
 *         the print head based off of the set
 *         nozzle data.
 */
void fireNozzle()
{ 
  //nozzle trigger for CD4067
  //check which nozzles are on,
  while((lastRowTime + 800) > micros())
  {
  }

  lastRowTime = micros();

  noInterrupts();
  for (uint8_t nozzle = 0; nozzle < 12; nozzle++)
  {
    if ((nozzleData[0] >> nozzle) & 1)
    {
      uint8_t offState = PORTA;
      uint8_t onState = PORTA | (((nozzle + 1) & 1) << 1);
      onState |= (((nozzle+ 1) >> 1) & 1) << 3;
      onState |= (((nozzle+ 1) >> 2) & 1) << 5;
      onState |= (((nozzle+ 1) >> 3) & 1) << 7;

      //turn on nozzle n
      PORTA = onState;

      //wait nozzle printing time
      delayMicroseconds(6);

      //turn off nozzle n
      PORTA = offState;

      delayMicroseconds(5);
    }
  }
  interrupts();
}



/*! \brief Fires a single nozzle in the print head
 *         which is decided in the settings.h file
 */
void fireSingleNozzle(){
  uint64_t oldNozzleData[2];

  for(uint8_t index = 0; index < 2; index++)
  {
    oldNozzleData[index] = nozzleData[index];
    nozzleData[index] = 0;
  }

  nozzleData[0] = 1 << optSingleNozzleSweepNozzle;
    
  fireNozzle();

  for(uint8_t index = 0; index < 2; index++)
  {
    nozzleData[index] = oldNozzleData[index];
  }
}



void setHeaterGoal(uint8_t newHeaterGoal)
{
  heaterGoal = newHeaterGoal;
}



float getHeatertempp()
{
  return ((float)analogRead(HEATER_SWEEP_SENSOR_PIN) / 1023.0) * 5.0;
}



void updateHeater()
{
  float heatData = analogRead(HEATER_SWEEP_SENSOR_PIN);

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
