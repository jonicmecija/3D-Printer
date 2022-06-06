//--------------------LIBRARY INCLUDES--------------------//
#include "ControlLoop.h"
#include "Print.h"
#include "Arduino.h"
#include "GCode.h"
#include "Buffer.h"
#include "USBSource.h"
#include "Motor.h"
#include "Nozzle.h"
#include "SDSource.h"
#include "Arduino.h"

//--------------------VARIABLES--------------------//
static uint8_t isPrintingSD = 0;
static uint8_t printerDwellTime = 0;
static uint8_t doNotExecute = 1;


//--------------------FUNCTIONS--------------------//
/*! \brief Stops the current sd card print sequence
 * 
 */
void stopSDPrint()
{
  isPrintingSD = 0;
}


uint8_t isPrinting()
{
  return isPrintingSD;
}


/*! \brief Starts the current sd card print sequence
 * 
 */
void startSDPrint(uint8_t isDryPrint, char* fileName)
{
  isPrintingSD = 1;
  initPrintBed(fileName, isDryPrint);
}


/*! \brief The main control loop for the program
 *         checks for new commands and executes them
 *         if there are any
 *         
 *         Execute this function as often as possble
 */
void controlLoop()
{
  if(isPrintingSD)
  {
    isPrintingSD = !sdPrint();
  }else if(isInM28Mode)
  {
    char* command = Serial.readStringUntil("\n").c_str();
    if(command[0] == 'M' && command[1] == '2' && command[2] == '9')
    {
      clearM28Mode();
    }
    else
    {
      writeFile(command);
    }
  }
  else
  {
    if(Serial.available())
    {
      resetCommandList();
      doNotExecute = 0;
      buffer_fill(0);
    }

    
    if(millis() > printerDwellTime && (!doNotExecute) && !isInM28Mode)
    {
      printerDwellTime = 0;

      updateG0();
      updateG1();
      updateG2();
      updateG3();
      printerDwellTime = updateG4();
      updateG20();
      updateG21();
      updateG28();
      updateG90();
      updateG91();
      updateG92();
      updateM3();
      updateM4();
      updateM5();
      updateM20();
      updateM23();
      updateM28();
      updateM92();
      updateM104();
      updateM105();
      updateM106();
      updateM107();
      //updateM109();
      //updateM190();

      
      if(updateM190 && updateM109())
      {
        buffer_next(0);
      }
  
      if(getBufferEmpty())
      {
        doNotExecute = 1;
        returnOkSignal();
      }
    }

    #ifdef DETERMINISTIC_MOTORS
    updateGantry();
    #endif

    #ifdef PRINT_TYPE_INKJET
    updateHeater();
    #endif  

    #ifdef PRINT_TYPE_FDM
    updateHeater();
    updateBed();
    updateExtruderMotor();
    #endif
  }
}
