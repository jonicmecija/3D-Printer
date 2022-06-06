#include "Settings.h"
#include "Motor.h"
#include "Pins.h"
#include <AccelStepper.h>
#include "Input.h"
#include "Math.h"


// The arduino mega can run 4000 steps per second since it runs a 16Mhz processor
#define MAX_MOTOR_SPEED 4000
boolean GHomed = 0;
boolean SHomed = 1; //TODO:replace this
boolean F1Homed = 0;
boolean BHomed = 0;
boolean target_reached;
boolean X_constant_movement = 0; //defines when X is on targeted movement (0) or constant movement (1) or arc movement (2)
boolean single_nozzle_printing;
boolean Out_of_feed_material = 0;

uint8_t arcFinished = 1;
uint8_t arcDirection = 0;
uint32_t arcDistance = 0;
uint32_t arcFeedRate = 0;
int32_t arcCenterX = 0;
int32_t arcCenterY = 0;
double arcStartAngle = 0;
double arcEndAngle = 0;
double endAngle = 0;

// variables for motor speed
int32_t xMotorSpeed = 0;
int32_t yMotorSpeed = 0;
int32_t zMotorSpeed = 0;




AccelStepper Xstepper(AccelStepper::DRIVER, X_MOTOR_STEP, X_MOTOR_DIRECTION, false);
AccelStepper Ystepper(AccelStepper::DRIVER, Y_MOTOR_STEP, Y_MOTOR_DIRECTION, false);
AccelStepper Zstepper(AccelStepper::DRIVER, Z_MOTOR_STEP, Z_MOTOR_DIRECTION, false);

AccelStepper Sstepper(AccelStepper::DRIVER, S_MOTOR_STEP, S_MOTOR_DIRECTION, false);
AccelStepper Fstepper(AccelStepper::DRIVER, F_MOTOR_STEP, F_MOTOR_DIRECTION, false);
AccelStepper Bstepper(AccelStepper::DRIVER, B_MOTOR_STEP, B_MOTOR_DIRECTION, false);



uint8_t homeAxis(AccelStepper* stepper, uint8_t endstop, 
                 int16_t homeSpeed, uint16_t backstep, int32_t offset);



void moveArc(int32_t I_10um, int32_t J_10um, uint8_t dir, int32_t feedrate)
{
  arcFinished = 0;

  arcStartAngle = atan(getMotorPosition(X_MOTOR)/getMotorPosition(Y_MOTOR));
  arcEndAngle = atan(I_10um/J_10um);

  arcDirection = dir;

  arcFeedRate = feedrate;
}


uint8_t getTargetReached()
{
  return target_reached;
}



int32_t getMotorPosition(uint8_t axis)
{
  switch(axis)
  {
    case X_MOTOR:
      return (Xstepper.currentPosition()/(float)optXStepsPermm) * 100;
    break;
    case Y_MOTOR:
      return (Ystepper.currentPosition()/(float)optYStepsPermm) * 100;
    break;
    case Z_MOTOR:
      return (Zstepper.currentPosition()/(float)optZStepsPermm) * 100;
    break;
    case F_MOTOR:
      return (Fstepper.currentPosition()/(float)optFStepsPermm) * 100;
    break;
    case S_MOTOR:
      return (Sstepper.currentPosition()/(float)optSStepsPermm) * 100;
    break;
    case B_MOTOR:
      return (Bstepper.currentPosition()/(float)optBStepsPermm) * 100;
    break;
  }
}



void setMotorPosition(uint8_t axis, int32_t newValue)
{
  switch(axis)
  {
    case X_MOTOR:
      return Xstepper.setCurrentPosition(newValue);
    break;
    case Y_MOTOR:
      return Ystepper.setCurrentPosition(newValue);
    break;
    case Z_MOTOR:
      return Zstepper.setCurrentPosition(newValue);
    break;
    case F_MOTOR:
      return Fstepper.setCurrentPosition(newValue);
    break;
    case S_MOTOR:
      return Sstepper.setCurrentPosition(newValue);
    break;
    case B_MOTOR:
      return Bstepper.setCurrentPosition(newValue);
    break;
  }
}



void homeMotor(uint8_t axis)
{
  Serial.println("Called");
  switch(axis)
  {
    case G_MOTORS:
      homeGantry();
    break;
    case F_MOTOR:
      homeAxis(&Fstepper, F_MOTOR_MIN_ENDSTOP, optFHomeSpeed, optFHomeBackstep, optFOffset);
    break;
    case B_MOTOR:
      homeAxis(&Bstepper, B_MOTOR_MIN_ENDSTOP, optBHomeSpeed, optBHomeBackstep, optBOffset);
    break;
    case S_MOTOR:
      homeAxis(&Sstepper, S_MOTOR_MIN_ENDSTOP, optSHomeSpeed, optSHomeBackstep, optSOffset);
    break;
    case X_MOTOR:
      homeAxis(&Xstepper, X_MOTOR_MIN_ENDSTOP, optGHomeSpeed, optGHomeBackstep, optXOffset);
    break;
    case Y_MOTOR:
      homeAxis(&Ystepper, Y_MOTOR_MIN_ENDSTOP, optGHomeSpeed, optGHomeBackstep, optYOffset);
    break;
    case Z_MOTOR:
      homeAxis(&Zstepper, Z_MOTOR_MIN_ENDSTOP, optGHomeSpeed, optGHomeBackstep, optZOffset);
    break;
  }
}


void enableMotors()
{
  Xstepper.enableOutputs();
  Ystepper.enableOutputs();
  Zstepper.enableOutputs();
  Sstepper.enableOutputs();
  Fstepper.enableOutputs();
  Bstepper.enableOutputs();
}


void disableMotors()
{
  Xstepper.disableOutputs();
  Ystepper.disableOutputs();
  Zstepper.disableOutputs();
  Sstepper.disableOutputs();
  Fstepper.disableOutputs();
  Bstepper.disableOutputs();
}


void initMotors()
{
  Xstepper.setEnablePin(X_MOTOR_ENABLE);
  Xstepper.setMaxSpeed(optTravelSpeed);
  Xstepper.setAcceleration(optTravelAccel);
  Xstepper.setPinsInverted(false, false, true);

  Ystepper.setEnablePin(Y_MOTOR_ENABLE);
  Ystepper.setMaxSpeed(optTravelSpeed);
  Ystepper.setAcceleration(optTravelAccel);
  Ystepper.setPinsInverted(false, false, true);

  Zstepper.setEnablePin(Z_MOTOR_ENABLE);
  Zstepper.setMaxSpeed(optTravelSpeed);
  Zstepper.setAcceleration(optTravelAccel);
  Zstepper.setPinsInverted(false, false, true);

  Sstepper.setEnablePin(S_MOTOR_ENABLE);
  Sstepper.setMaxSpeed(optSMotorSpeed);
  Sstepper.setAcceleration(optSMotorAccel);
  Sstepper.setPinsInverted(false, false, true);

  Fstepper.setEnablePin(F_MOTOR_ENABLE);
  Fstepper.setMaxSpeed(optFMotorSpeed);
  Fstepper.setAcceleration(optFMotorAccel);
  Fstepper.setPinsInverted(false, false, true);

  Bstepper.setEnablePin(B_MOTOR_ENABLE);
  Bstepper.setMaxSpeed(optBMotorSpeed);
  Bstepper.setAcceleration(optBMotorAccel);
  Bstepper.setPinsInverted(false, false, true);

  disableMotors();

  pinMode(X_MOTOR_MIN_ENDSTOP, INPUT);
  pinMode(X_MOTOR_MAX_ENDSTOP, INPUT);
  pinMode(Y_MOTOR_MIN_ENDSTOP, INPUT);
  pinMode(Y_MOTOR_MAX_ENDSTOP, INPUT);
  pinMode(Z_MOTOR_MIN_ENDSTOP, INPUT);
  pinMode(Z_MOTOR_MAX_ENDSTOP, INPUT);
  pinMode(F_MOTOR_MIN_ENDSTOP, INPUT);
  pinMode(F_MOTOR_MAX_ENDSTOP, INPUT);
  pinMode(S_MOTOR_MIN_ENDSTOP, INPUT);
  pinMode(S_MOTOR_MAX_ENDSTOP, INPUT);
  pinMode(B_MOTOR_MIN_ENDSTOP, INPUT);
  pinMode(B_MOTOR_MAX_ENDSTOP, INPUT);
}



void homeGantry()
{
  if(homeAxis(&Xstepper, X_MOTOR_MIN_ENDSTOP, optGHomeSpeed, optGHomeBackstep, optXOffset) & 
     homeAxis(&Ystepper, Y_MOTOR_MIN_ENDSTOP, optGHomeSpeed, optGHomeBackstep, optYOffset) &
     homeAxis(&Zstepper, Z_MOTOR_MIN_ENDSTOP, optGHomeSpeed, optGHomeBackstep, optZOffset)){
    GHomed = 1;
    target_reached = 1;
  }
}



uint8_t getXConstantMovement()
{
  return X_constant_movement;
}


uint8_t getPrintingMode()
{
  return single_nozzle_printing;
}

void setPrintingMode(uint8_t mode)
{
  single_nozzle_printing = mode;
}


uint8_t homeAxis(AccelStepper* stepper, uint8_t endstop, 
                 int16_t homeSpeed, uint16_t backstep, int32_t offset){
  #ifndef DETERMINISTIC_MOTORS
    TIMSK1 = 0x00; //Disable motor update interrupt
  #endif
  
  uint8_t interrupt = 0;
  int readcounter = 0;
  const int readtarget = 100;

  stepper->enableOutputs();
  stepper->stop();

  float oldSpeed = stepper->maxSpeed();

  
  for(uint8_t iteration = 0; iteration < 2 && !interrupt; iteration++){
    uint8_t axisDirection = -1;
    stepper->setMaxSpeed(homeSpeed);
    stepper->move(30000 * axisDirection);
    
    while(!digitalRead(endstop) && !interrupt){
      stepper->run();
      
      //Read ui inputs periodically
      if(readcounter > readtarget){
        updateInputs();
        readcounter = 0;
      }
      readcounter++; 

      if (getButton(BUTTON_LEFT) && getButton(BUTTON_RIGHT))
      {
        interrupt = 1;
        break;
      }
    }
    
    stepper->setSpeed(0);
    stepper->runSpeed();
    
    axisDirection = 1;
    stepper->setMaxSpeed(homeSpeed/5);

    if(iteration == 0)
    {
      stepper->move(backstep * axisDirection);
      stepper->runToPosition();
    }
  }

  stepper->setMaxSpeed(oldSpeed);
  stepper->setCurrentPosition(offset);

  if (interrupt){
    return false;
  }else{
    return true;
  }
}



void updateGantry()
{
  //update X axis
  Xstepper.run();
  Ystepper.run();
  Zstepper.run();


  if((!Xstepper.isRunning()) && (!Ystepper.isRunning()) && (!arcFinished))
  {
    if(arcStartAngle >= arcEndAngle){
      arcFinished = 1;
    }
    else{      
      int32_t newGoalX = arcDistance * cos(arcStartAngle) + arcCenterX;
      int32_t newGoalY = arcDistance * cos(arcStartAngle) + arcCenterY;
  
      setGantryTarget(newGoalX, newGoalY, getMotorPosition(Z_MOTOR), arcFeedRate);
  
      if(arcDirection)
      {
        arcStartAngle += optArcIncrementAmount;
      }else{
        arcStartAngle -= optArcIncrementAmount;
      }
    }  
  }
    

  if((!Xstepper.isRunning()) && (!Ystepper.isRunning()) && (!X_constant_movement) && arcFinished){
    target_reached = 1;
  }
}

/* NEW FUNCTION ADDED FOR PROPORTIONAL SPEEDS */
// inputs are X position, Y position, Z position, and feed rate speed

void getMotorSpeed(int32_t X_10um, int32_t Y_10um, int32_t Z_10um, int32_t feedrate){
  // feed rate does not used

  int32_t maxMotorSpeed = MAX_MOTOR_SPEED; // To Do: need to set the max motor speed in preprocessor directive

  // calculate target position
  int32_t stepPositionX;
  int32_t stepPositionY;
  int32_t stepPositionZ;
  stepPositionX = (float)X_10um * (float)0.01 * (float)optXStepsPermm;
  stepPositionY = (float)Y_10um * (float)0.01 * (float)optYStepsPermm;
  stepPositionZ = (float)Z_10um * (float)0.01 * (float)optZStepsPermm;

  // calculate the distance to travel in each axis
  int32_t distanceToTravelX = abs(Xstepper.currentPosition() - stepPositionX);
  int32_t distanceToTravelY = abs(Ystepper.currentPosition() - stepPositionY);
  int32_t distanceToTravelZ = abs(Zstepper.currentPosition() - stepPositionZ);

  // find which axis with longest distance to travel, and calculate proportional speeds for other axes

  if(distanceToTravelX > distanceToTravelY && distanceToTravelX > distanceToTravelZ){
    // X axis needs to travel greatest distance, set constant speed on X stepper motor
    xMotorSpeed = maxMotorSpeed;
    yMotorSpeed = maxMotorSpeed * (distanceToTravelY / distanceToTravelX);
    zMotorSpeed = yMotorSpeed * (distanceToTravelZ / distanceToTravelY);
  }
  else if(distanceToTravelY > distanceToTravelX && distanceToTravelY > distanceToTravelZ){
    // Y axis needs to travel greatest distance, set constant speed on Y stepper motor
    yMotorSpeed = maxMotorSpeed;
    xMotorSpeed = maxMotorSpeed * (distanceToTravelX / distanceToTravelY);
    zMotorSpeed = xMotorSpeed * (distanceToTravelZ / distanceToTravelX);
  }
  else if(distanceToTravelZ > distanceToTravelX && distanceToTravelZ > distanceToTravelY){
    // Z axis needs to travel greatest distance, set constant speed on Z stepper motor
    zMotorSpeed = maxMotorSpeed;
    xMotorSpeed = maxMotorSpeed * (distanceToTravelX / distanceToTravelZ);
    yMotorSpeed = xMotorSpeed * (distanceToTravelY / distanceToTravelX);
  }

}

void setGantryTarget(int32_t X_10um, int32_t Y_10um, int32_t Z_10um, int32_t feedrate)
{
  // Xstepper.currentPosition();

  if (target_reached == 1)
  { 
    target_reached = 0;
    // old code

    // int32_t stepPositionX;
    // int32_t stepPositionY;
    // int32_t stepPositionZ;
    // stepPositionX = (float)X_10um * (float)0.01 * (float)optXStepsPermm;
    // stepPositionY = (float)Y_10um * (float)0.01 * (float)optYStepsPermm;
    // stepPositionZ = (float)Z_10um * (float)0.01 * (float)optZStepsPermm;
    // Xstepper.setMaxSpeed((float)feedrate * (float)optXStepsPermm);
    // Ystepper.setMaxSpeed((float)feedrate * (float)optYStepsPermm);
    // Zstepper.setMaxSpeed((float)feedrate * (float)optZStepsPermm);

    // calculate speed before setting speed and acceleration
    getMotorSpeed(X_10um, Y_10um,  Z_10um, feedrate);

    // change max speed - so that each speed is proportional to eachother
    Xstepper.setMaxSpeed(xMotorSpeed);
    Ystepper.setMaxSpeed(yMotorSpeed);
    Zstepper.setMaxSpeed(zMotorSpeed);

    // change acceleration - setAcceleration()
    Xstepper.setAcceleration(xMotorSpeed);
    Ystepper.setAcceleration(yMotorSpeed);
    Zstepper.setAcceleration(zMotorSpeed);

    Xstepper.moveTo(stepPositionX);
    Ystepper.moveTo(stepPositionY);
    Zstepper.moveTo(stepPositionZ);
  }  
}



void startSweepX(uint8_t X_start_dir)
{
  if (X_constant_movement == 0)
  {
    uint8_t directionValue;
    if(X_start_dir){
      directionValue = 1;
    }else{
      directionValue = -1;
    }
    Xstepper.setMaxSpeed(optNozzleSweepSpeed);
    Xstepper.moveTo(30000 * directionValue);
    X_constant_movement = 1;
    target_reached = 0;
  }
}


void stopSweepX()
{
  if (X_constant_movement == 1)
  {
    X_constant_movement = 0;
    Xstepper.setMaxSpeed(optTravelSpeed);
    Xstepper.setSpeed(0);
    Xstepper.runSpeed();
    target_reached = 1;
  }
}



void newLayer(int NL_X_target, int NL_Y_target, int layer_thickness)
{
  int New_layer_ident = 0;
  
  /*if (SHomed == 1 && GHomed == 1) // are important axes homed?
  {
    //Move gantry to homing point
    Xstepper.moveTo(optXOffset);
    Ystepper.moveTo(optYOffset);
    
    while(Xstepper.isRunning() || Ystepper.isRunning()){}


    //Lift print head to avoid sweeper
    Zstepper.moveTo(optZLiftHeight);
    Zstepper.runToPosition();


    //Let the heater warm up
    //start_layer_heater();


    //Sweep across the bed to warm previous layer
    uint16_t previousXSpeed = Xstepper.maxSpeed();
    Xstepper.setMaxSpeed(optHeaterTravelSpeed);
    
    Xstepper.moveTo((long) optSTravelDistance);
    while(Xstepper.isRunning());

    Xstepper.moveTo(optXOffset);
    while(Xstepper.isRunning());

    Xstepper.setMaxSpeed(previousXSpeed);


    //Let the heater cool down
    //stop_layer_heater();


    //Move the feed and build tanks
    Fstepper.move(layer_thickness * optSStepsPermm);
    Bstepper.move(-layer_thickness * optBStepsPermm);

    while(Fstepper.isRunning() || Bstepper.isRunning())
    {
      Fstepper.run();
      Bstepper.run();
    }


    //Check if we still have material for next layer
    if(digitalRead(F_MOTOR_MAX_ENDSTOP))
    {
        Out_of_feed_material = 1;
    }


    //Move sweeper out and back in
    Sstepper.moveTo(optSTravelDistance * optSStepsPermm);
    Sstepper.runToPosition();


    Sstepper.moveTo(0);
    Sstepper.runToPosition();


    //Lower print head
    Zstepper.moveTo(0);
    while(Zstepper.isRunning());


    //Move the print head to the target location
    Xstepper.moveTo(NL_X_target * 0.01 * 20);
    Ystepper.moveTo(NL_Y_target * 0.01 * 20);
    
    while(Xstepper.isRunning() || Ystepper.isRunning()){}
  }*/
}
