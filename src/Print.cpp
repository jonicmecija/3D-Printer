#include "Arduino.h"
#include "Pins.h"
#include "Settings.h"
#include "Buffer.h"
#include "GCodeParser.h"
#include "Nozzle.h"
#include "SDSource.h"
#include "Input.h"
#include "Menu.h"
#include "Motor.h"
#include "Print.h"
#include "GCode.h"
#include "ControlLoop.h"



static uint32_t printerDwellTime = 0;

static uint8_t printerActive = 0;
static uint8_t printDirection = 0;
static int32_t depositTarget = 0;
static int32_t nozzleChangePoint = 0;
static uint8_t dryPrint = 0;
static uint8_t printPaused = 0;



void initPrintBed(char* fileName, uint8_t isDryPrint)
{
    dryPrint = isDryPrint;
  
    //Home all print axis
    homeGantry();
    //Home_S();
    
    
    //Initialize print file
    openPrintFile(fileName);
    
    
    //Initialize the buffer
    buffer_fill(1);
    
    
    //Start the print head
    turnOnNozzle();
}



void updateNozzleSweepingCommand()
{
  #ifdef PRINT_TYPE_INKJET
    if (getXConstantMovement())
    {   
        if((nozzleChangePoint > getMotorPosition(X_MOTOR) && !printDirection) ||
           (nozzleChangePoint < getMotorPosition(X_MOTOR) && printDirection))
        {
          
          if(dryPrint){
              uint64_t tempZeroArray[2] = {0};
              setNozzleData(tempZeroArray);
          }else{
              setNozzleData(getNozzleState());
          }
          
          
          buffer_next(1);
          
  
          if(getBufferCoordinates()[0] != 2000)
          {
            stopSweepX();
          }
  
          
          else
          {
            int32_t newXAxis = getBufferCoordinates()[1];
            checkAxisParamaters(&newXAxis, X_MOTOR);
            
            if(getBufferCoordinates()[0] == 2000)
            {
                nozzleChangePoint = newXAxis;
            }
          }
        }
        
        
        if((depositTarget > getMotorPosition(X_MOTOR) && !printDirection) ||
           (depositTarget < getMotorPosition(X_MOTOR) && printDirection))
        {
            loadNozzle();
            fireNozzle();

            //set next target
            if(printDirection)
            {
                depositTarget += (1.0/optNozzleDPI) * 25.4 * 100;
            }else{
                depositTarget -= (1.0/optNozzleDPI) * 25.4 * 100;
            }
        }
    }
  #endif
}


uint8_t sdPrint()
{    
  uint32_t targetLoopTime = micros() + 450;
  if(getTargetReached() && (millis() > printerDwellTime))
  {
    
    printerDwellTime = 0;

    printPaused = updateM0(printPaused);
    
    if(!printPaused)
    {
      uint8_t prevPrintDirection = printDirection;
      printDirection = updateG0();
      printDirection = updateG1();
      if(printDirection == 3)
      {
        printDirection = prevPrintDirection;
      }
      
   
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
      updateM92();
      updateM104();
      updateM105();
      updateM106();
      updateM107();
      updateM109();
      updateM190();
    }
    
    #ifdef PRINT_TYPE_INKJET
      //Execute nozzle command
      if(getBufferCoordinates()[0] == 2000)
      {
        int32_t newXAxis = getBufferCoordinates()[1];
        checkAxisParamaters(&newXAxis, X_MOTOR);
        
        if(printDirection)
        {
          depositTarget = newXAxis + ((1.0/optNozzleDPI) * 25.4);
        }else{
          depositTarget = newXAxis - ((1.0/optNozzleDPI) * 25.4);
        }

        startSweepX(printDirection);
        nozzleChangePoint = newXAxis;
      }
    #endif

    if(!getXConstantMovement() && updateM190() && updateM109())
    {
      buffer_next(1);
    }
  }


  #ifdef PRINT_TYPE_INKJET
  //updateHeater();
  updateNozzleSweepingCommand();
  #endif  
  

  #ifdef PRINT_TYPE_FDM
  updateHeater();
  updateBed();
  updateExtruderMotor();
  #endif


  while((getEmptySlots() > 100))
  {
    buffer_update();
  }

  
  while((getEmptySlots() > 0) && (targetLoopTime > micros()))
  {
      buffer_update();
  }

  if(!printPaused)
  {
    updateGantry();
  }
  
 
  if (isEndOfFile() && (getEmptySlots() == 0))
  {
      turnOffNozzle();
      stopSDPrint();
      printerActive = 0;
      
      turnOffNozzle();
      
      //close file
      closeFile();
      stopSweepX();
      
      //return gantry to home possition
      homeGantry();
      disableMotors();

      return 1;
  }

  return 0;
}
