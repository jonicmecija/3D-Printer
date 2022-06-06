//--------------------LIBRARY INCLUDES--------------------//
#include "Arduino.h"
#include "Pins.h"
#include "Settings.h"
#include "Buffer.h"
#include "Motor.h"
#include "GCode.h"
#include "Nozzle.h"
#include "SDSource.h"
#include "Print.h"

//--------------------VARIABLES--------------------//
static double conversionFactor = 1;
static uint8_t isAbsolute = 1;
static uint8_t ignoreFan[2] = {0};
static int32_t zHeight = 0;

uint8_t isInM28Mode = 0;


//--------------------HELPER FUNCTIONS--------------------//
/*! \brief Checks if the mode bits (2 most significant bits)
 *         are equal to the given mode parameter. Returns
 *         1 if true and 0 if fase.
 */
int8_t checkIfMode(int32_t value, uint8_t mode)
{
  return ((value >> 30) & 3) == mode;
}


/*! \brief Strips the mode bits (2 most significant bits) 
 *         from the given value, returning the coordinate value
 */
int32_t getCoordinateValue(uint32_t value)
{
  if((value >> 29) & 1)
  {
    return value | ((int32_t)3 << 30);
  }

  else
  {
    return value & ~((int32_t)3 << 30);
  }
}


//--------------------FUNCTIONS--------------------//
/*! \brief Calculate the goal coordinates from the given
 *         command cordinate value
 *         
 *         This function first checks if there is a valid
 *         coordinate and if not returns the current position.
 *         Otherwise it strips the mode bits and applies the
 *         current conversion factor
 */
void checkAxisParamaters(int32_t* newValue, uint8_t motorAxis)
{
  if(isAbsolute)
  {
    if(checkIfMode(*newValue, 0))
    {
      *newValue = getMotorPosition(motorAxis) * conversionFactor;
    }else{
      *newValue = getCoordinateValue(*newValue) * conversionFactor;
    }
  }else{
    if(checkIfMode(*newValue, 0))
    {
      *newValue = getMotorPosition(motorAxis) * conversionFactor;
    }else{
      *newValue = getMotorPosition(motorAxis) + getCoordinateValue(*newValue) * conversionFactor;
    }
  }
}


/*! \brief Checks if the current command is a G1 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is INKJET then this command
 *         takes the x, y, z, and f parameters and moves
 *         the head to the xyz position at the speed f.
 *         
 *         If PRINT_HEAD_TYPE is LASER then this command
 *         takes the x, y, z, and f parameters and does
 *         a non-lasered move to the xyz position at the 
 *         speed f.
 */
uint8_t updateG0()
{
  if(getBufferCoordinates()[0] == 1000)
  {
    return updateG1();
  }

  return 0;
}


/*! \brief Checks if the current command is a G1 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is INKJET then this command
 *         takes the x, y, z, and f parameters and moves
 *         the head to the xyz position at the speed f.
 *         
 *         If PRINT_HEAD_TYPE is LASER then this command
 *         takes the x, y, z, and f parameters and does
 *         a lasered move to the xyz position at the speed f.
 */
uint8_t updateG1()
{
  int32_t newXAxis = getBufferCoordinates()[1];
  int32_t newYAxis = getBufferCoordinates()[2];
  int32_t newZAxis = getBufferCoordinates()[3];
  checkAxisParamaters(&newXAxis, X_MOTOR);
  checkAxisParamaters(&newYAxis, Y_MOTOR);
  checkAxisParamaters(&newZAxis, Z_MOTOR);

  //Execute move to position code
  if(getBufferCoordinates()[0] == 1001 | getBufferCoordinates()[0] == 1000)
  {  
    
    if(checkIfMode(getBufferCoordinates()[5], 1)) //Check for f axis
    {
      optTravelSpeed = getCoordinateValue(getBufferCoordinates()[5]) / 100;
    }

    #ifdef PRINT_TYPE_LASER
      if(getBufferCoordinates()[0] == 1000)
      {
        turnOffNozzle();
      }else{
        turnOnNozzle();
      }
    #endif

    setGantryTarget(newXAxis, newYAxis, newZAxis, optTravelSpeed);
  }
  
  
  //Execute directed move to position code
  if(getBufferCoordinates()[0] == 999 || getBufferCoordinates()[0] == 998)
  {
    
    if(checkIfMode(getBufferCoordinates()[5], 1)) //Check for f axis
    {
      optTravelSpeed = getCoordinateValue(getBufferCoordinates()[5]) / 100;
    }

    #ifdef PRINT_TYPE_LASER
      if(getBufferCoordinates()[0] == 1000)
      {
        turnOffNozzle();
      }else{
        turnOnNozzle();
      }
    #endif

    setGantryTarget(newXAxis, newYAxis, newZAxis, optTravelSpeed);

    return getBufferCoordinates()[0] == 998;
  }  

  #ifdef PRINT_TYPE_FDM
    if(checkIfMode(getBufferCoordinates()[4], 1)) //Check for e axis
    {
      moveExtruder(getCoordinateValue(getBufferCoordinates()[4]));
    }
  #endif
  
  return 3;
}


/*! \brief Checks if the current command is a G1 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is INKJET then this command
 *         takes the x, y, z, and f parameters and moves
 *         the head in an arc to the xyz position at the speed f.
 *         
 *         If PRINT_HEAD_TYPE is LASER then this command
 *         takes the x, y, z, and f parameters and does
 *         a lasered arc move to the xyz position at the speed f.
 */
void updateG2(){
  if(getBufferCoordinates()[0] == 1003)
  {
    if(checkIfMode(getBufferCoordinates()[3], 2) && //Check for i axis
       checkIfMode(getBufferCoordinates()[6], 3) && //Check for j axis
       checkIfMode(getBufferCoordinates()[5], 1))   //Check for f axis
    {
      int32_t newIValue = getCoordinateValue(getBufferCoordinates()[3]);
      int32_t newJValue = getCoordinateValue(getBufferCoordinates()[6]);
      int32_t newFeedRate = getCoordinateValue(getBufferCoordinates()[5]);
    
      moveArc(newIValue, newJValue, 0, newFeedRate);
    }  
  }
}


/*! \brief Checks if the current command is a G1 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is INKJET then this command
 *         takes the x, y, z, and f parameters and moves
 *         the head in an arc to the xyz position at the speed f.
 *         
 *         If PRINT_HEAD_TYPE is LASER then this command
 *         takes the x, y, z, and f parameters and does
 *         a lasered arc move to the xyz position at the speed f.
 */
void updateG3(){
  if(getBufferCoordinates()[0] == 1003)
  {
    if(checkIfMode(getBufferCoordinates()[3], 2) && //Check for i axis
       checkIfMode(getBufferCoordinates()[6], 3) && //Check for j axis
       checkIfMode(getBufferCoordinates()[5], 1))   //Check for f axis
    {
      int32_t newIValue = getCoordinateValue(getBufferCoordinates()[3]);
      int32_t newJValue = getCoordinateValue(getBufferCoordinates()[6]);
      int32_t newFeedRate = getCoordinateValue(getBufferCoordinates()[5]);
    
      moveArc(newIValue, newJValue, 1, newFeedRate);
    }  
  }
}


/*! \brief Checks if the current command is a G4 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         the parameters s or p. If the parameter is s, then
 *         the print pauses for s seconds. If the parameter is
 *         p then the print pauses for p miliseconds.
 */
uint32_t updateG4()
{
  if(getBufferCoordinates()[0] == 1004)
  {
    if(checkIfMode(getBufferCoordinates()[6], 1)) //Check for s axis
    {
      return millis() + (getCoordinateValue(getBufferCoordinates()[6]) / 100) * 1000;
    }
    else if(checkIfMode(getBufferCoordinates()[5], 2)) //check for p axis
    {
      return millis() + getCoordinateValue(getBufferCoordinates()[5]) / 100;
    }
  }

  return 0;
}


/*! \brief Checks if the current command is a G20 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         no parameters and changes the conversion factor
 *         so that all commands are in inches
 */
void updateG20()
{
  if(getBufferCoordinates()[0] == 1020)
  {
    conversionFactor = 25.4;
  }
}


/*! \brief Checks if the current command is a G21 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         no parameters and changes the conversion factor
 *         so that all commands are in milimeters
 */
void updateG21()
{
  if(getBufferCoordinates()[0] == 1021)
  {
    conversionFactor = 1;
  }
}


/*! \brief Checks if the current command is a G28 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         the parameters x, y, and z and homes the axis that
 *         are listed in the command
 */
void updateG28()
{
  if(getBufferCoordinates()[0] == 1028)
  {
    if(checkIfMode(getBufferCoordinates()[1], 1)) //Check for x axis
    {
      homeMotor(X_MOTOR);
    }
    
    if(checkIfMode(getBufferCoordinates()[2], 1)) //Check for y axis
    {
      homeMotor(Y_MOTOR);
    }
    
    if(checkIfMode(getBufferCoordinates()[3], 1)) //Check for z axis
    {
      homeMotor(Z_MOTOR);
    }

    if(checkIfMode(getBufferCoordinates()[1], 0) && //Check for x axis
       checkIfMode(getBufferCoordinates()[2], 0) && //Check for y axis
       checkIfMode(getBufferCoordinates()[3], 0))   //Check for z axis
    {
      homeMotor(X_MOTOR);
      homeMotor(Y_MOTOR);
      homeMotor(Z_MOTOR);
    }

    
  }
}


/*! \brief Checks if the current command is a G90 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         no parameters and makes the printer operate with
 *         absolute coordinates
 */
void updateG90()
{
  if(getBufferCoordinates()[0] == 1090)
  {
    isAbsolute = 1;
  }
}


/*! \brief Checks if the current command is a G91 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         no parameters and makes the printer operate with
 *         relative coordinates
 */
void updateG91()
{
  if(getBufferCoordinates()[0] == 1091)
  {
    isAbsolute = 0;
  }
}


/*! \brief Checks if the current command is a G4 command
 *         and executes it.
 *         
 *         For all PRINT_HEAD_TYPE values this command takes
 *         the parameters x, y, or z. It then resets the printers
 *         location to the value given for that parameter
 */
void updateG92()
{
  if(getBufferCoordinates()[0] == 1092)
  {
    if(checkIfMode(getBufferCoordinates()[1], 1)) //Check for x axis
    {
      homeMotor(X_MOTOR);
      setMotorPosition(X_MOTOR, getCoordinateValue(getBufferCoordinates()[1]));
    }
    
    if(checkIfMode(getBufferCoordinates()[2], 1)) //Check for y axis
    {
      homeMotor(Y_MOTOR);
      setMotorPosition(Y_MOTOR, getCoordinateValue(getBufferCoordinates()[2]));
    }
    
    if(checkIfMode(getBufferCoordinates()[3], 1)) //Check for z axis
    {
      homeMotor(Z_MOTOR);
      setMotorPosition(Z_MOTOR, getCoordinateValue(getBufferCoordinates()[3]));
    }

    #ifdef PRINT_TYPE_FDM
      if(checkIfMode(getBufferCoordinates()[4], 1)) //Check for E axis
      {
        homeMotor(Z_MOTOR);
        setExtruderPosition(getCoordinateValue(getBufferCoordinates()[4]));
      }
    #endif
    
  }
}



/*! \brief Checks if the current command is a M3 command
 *         and executes it.
 *         
 *         If the current print is paused, then unpause
 *         the print. If the print is not paused then
 *         pause the print
 */
uint8_t updateM0(uint8_t isPrintPaused)
{
  if((getBufferCoordinates()[0] == 226))
  {
    return !isPrintPaused;
    Serial.println("Pasue");
  }else{
    return isPrintPaused;
  }
}


/*! \brief Checks if the current command is a M3 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is LASER then this command
 *         takes the value between 0-255 from the s
 *         parameter and sets that as the new laser power
 *         
 *         If PRINT_HEAD_TYPE is SPINDLE then this commnad
 *         makes the spindle rotate clockwise
 *         
 *         For all other PRINT_HEAD_TYPE this command does
 *         nothing
 */
void updateM3()
{
  if(getBufferCoordinates()[0] == 3)
  {
    if(checkIfMode(getBufferCoordinates()[6], 1)) //Check for s axis
    {
      #ifdef PRINT_TYPE_LASER
        if(getCoordinateValue(getBufferCoordinates()[6] < 256))
        {
          setNozzlePower(getCoordinateValue(getBufferCoordinates()[6]));
          turnOnNozzle();
        }
      #endif

      #ifdef PRINT_TYPE_MILL
        setNozzlePower(1);
        turnOnNozzle();
      #endif
    }    
  }
}


/*! \brief Checks if the current command is a M4 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is SPINDLE then this commnad
 *         makes the spindle rotate counter-clockwise
 *         
 *         For all other PRINT_HEAD_TYPE this command does
 *         nothing
 */
void updateM4()
{
  if(getBufferCoordinates()[0] == 4)
  {
    #ifdef PRINT_TYPE_MILL
        setNozzlePower(-1);
        turnOnNozzle();
      #endif
  }
}


/*! \brief Checks if the current command is a M5 command
 *         and executes it.
 *         
 *         If PRINT_HEAD_TYPE is SPINDLE or LASER then this 
 *         commnad turns of the laser/spindle head
 */
void updateM5()
{
  if(getBufferCoordinates()[0] == 5)
  {    
    #if defined(PRINT_TYPE_MILL) || defined(PRINT_TYPE_LASER)
        turnOffNozzle();
      #endif
  }
}



void updateM20()
{
  if(getBufferCoordinates()[0] == 20)
  {    
    char fileName[20] = {0};
    
    Serial.println("Begin file list");
    startFileNameList();
    while(!getNextFileName(fileName))
    {
      Serial.println(fileName);
    }
    endFileNameList();
    Serial.println("End file list");
  }
}



void updateM23()
{
  if(getBufferCoordinates()[0] == 23)
  {    
    if(checkIfMode(getBufferCoordinates()[1], 1)) //Check file pointer
    {
      initPrintBed(getCoordinateValue(getBufferCoordinates()[1]), 0);
      while(!sdPrint());
    }
  }
}


void updateM28()
{
  if(getBufferCoordinates()[0] == 28)
  {    
    if(checkIfMode(getBufferCoordinates()[1], 1)) //Check file pointer
    {
      createFile(getBufferCoordinates()[1]);
      isInM28Mode = 0;
    }
  }
}


void clearM28Mode()
{
  isInM28Mode = 0;
  closeFile();
}


void updateM92()
{
  if(getBufferCoordinates()[0] == 92)
  { 
    if(checkIfMode(getBufferCoordinates()[1], 1)) //Check x axis
    {
      optXStepsPermm = getCoordinateValue(getBufferCoordinates()[1])/100;
    }
    
    if(checkIfMode(getBufferCoordinates()[2], 1)) //Check y axis
    {
      optYStepsPermm = getCoordinateValue(getBufferCoordinates()[2])/100;
    }
    
    if(checkIfMode(getBufferCoordinates()[3], 1)) //Check z axis
    {
      optZStepsPermm = getCoordinateValue(getBufferCoordinates()[3])/100;
    }
  }  
}



void updateM104()
{
  if(getBufferCoordinates()[0] == 104)
  {    
    #if defined(PRINT_TYPE_INKJET) || defined(PRINT_TYPE_FDM)
    if(checkIfMode(getBufferCoordinates()[6], 1)) //Check s axis
    {
      setHeaterGoal(getCoordinateValue(getBufferCoordinates()[6]));
    }
    #endif
  }
}



void updateM105()
{
  if(getBufferCoordinates()[0] == 105)
  {   
    #if defined(PRINT_TYPE_INKJET) || defined(PRINT_TYPE_FDM)

    Serial.print("T:");
    //Serial.print(getHeatertempp());
    Serial.print(millis() % 100);
    Serial.print(" /");
    Serial.print(200);

    #ifdef PRINT_TYPE_FDM
    Serial.print(" B:");
    //Serial.print(getBedtemp());
    Serial.print(millis() % 100);
    Serial.print(" /");
    Serial.println(200);
    #else
    Serial.println();
    #endif  

    #endif
  }
}



void updateM106()
{
  #ifdef PRINT_TYPE_FDM
  if(getBufferCoordinates()[0] == 106)
  {
    uint8_t currentFan = 0;

    if(checkIfMode(getBufferCoordinates()[5], 2)) //Check p axis
    {
      currentFan = getCoordinateValue(getBufferCoordinates()[5]);
    }

    if(checkIfMode(getBufferCoordinates()[3], 2)) //Check i axis
    {
      if(currentFan){
        ignoreFan[0] = getCoordinateValue(getBufferCoordinates()[3]);
      }else{
        ignoreFan[1] = getCoordinateValue(getBufferCoordinates()[3]);
      }
    }

    if(checkIfMode(getBufferCoordinates()[6], 1)) //Check s axis
    {
      if(!currentFan && !ignoreFan[0])
      {
        setHeaterFanSpeed(getCoordinateValue(getBufferCoordinates()[6]));
      }else if(currentFan && !ignoreFan[1]){
        setFilamentFanSpeed(getCoordinateValue(getBufferCoordinates()[6]));
      }
    }
  }
  #endif
}



void updateM107()
{
  #ifdef PRINT_TYPE_FDM
  if(getBufferCoordinates()[0] == 107)
  {
    uint8_t currentFan = 0;

    if(checkIfMode(getBufferCoordinates()[5], 2)) //Check p axis
    {
      currentFan = getCoordinateValue(getBufferCoordinates()[5]);
    }

    if(!currentFan && !ignoreFan[0])
    {
      setHeaterFanSpeed(0);
    }
    else if(currentFan && !ignoreFan[1])
    {
      setFilamentFanSpeed(0);
    }
  }
  #endif 
}



uint8_t updateM109()
{
  #ifdef PRINT_TYPE_INKJET
  if(getBufferCoordinates()[0] == 190)
    {
      if(checkIfMode(getBufferCoordinates()[5], 2)) //
      {
        setHeaterGoal(getCoordinateValue(getBufferCoordinates()[5]));
        
        if(getHeatertempp() >= getCoordinateValue(getBufferCoordinates()[5]))
        {
          return 1;
        }
        else
        {
          return 0;
        }  
      } 
    }
  #endif
  return 1;
}



uint8_t updateM190()
{
  #ifdef PRINT_TYPE_FDM
  if(getBufferCoordinates()[0] == 190)
    {
      if(checkIfMode(getBufferCoordinates()[6], 1)) //Check s axis
      {
        setBedGoal(getCoordinateValue(getBufferCoordinates()[6]));
        
        if(getBedTemp() >= getCoordinateValue(getBufferCoordinates()[6]))
        {
          return 1;
        }
        else
        {
          return 0;
        }  
      } 
    }
  #endif
  return 1;
}
