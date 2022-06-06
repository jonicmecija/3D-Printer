#include "Settings.h"
#include "Pins.h"
#include "Nozzle.h"
#include "AccelStepper.h"

#ifdef FDM

uint16_t heaterGoal = 0;
uint16_t bedGoal = 0;
uint8_t heaterFanSpeed = 0;
uint8_t filamentFanSpeed = 0;
uint8_t extruderAbsolute = 1;

AccelStepper extruderStepper(AccelStepper::DRIVER, EXTRUDER_STEP, EXTRUDER_DIRECTION, false);


void turnOnNozzle(){
  
}


void turnOffNozzle(){
  
}


void initNozzle()
{
  pinMode(FDM_HEATER_FAN_PIN, OUTPUT);
  digitalWrite(FDM_HEATER_FAN_PIN, 0);
  pinMode(FDM_FILAMENT_FAN_PIN, OUTPUT);
  digitalWrite(FDM_FILAMENT_FAN_PIN, 0);
  pinMode(HEATER_SWEEP_PIN, OUTPUT);
  digitalWrite(HEATER_SWEEP_PIN, LOW);
  pinMode(HEATER_BED_PIN, OUTPUT);
  digitalWrite(HEATER_BED_PIN, LOW);
  extruderStepper.setEnablePin(EXTRUDER_MOTOR_ENABLE);
  extruderStepper.setMaxSpeed(optExtruderMotorSpeed);
  extruderStepper.setAcceleration(optExtruderMotorAccel);
  extruderStepper.setPinsInverted(false, false, true);
  extruderStepper.enableOutputs();
}



void updateExtruderMotor()
{
  extruderStepper.run();
}


void moveExtruder(int32_t position){
  if(extruderAbsolute)
  {
    extruderStepper.moveTo(((float)position * 0.01) * optExtruderStepsPermm);
  }
  else
  {
    extruderStepper.move(((float)position * 0.01) * optExtruderStepsPermm);
  }
}


void setExtruderAbsolute()
{
  extruderAbsolute = 1;
}



void setExtruderRelative()
{
  extruderAbsolute = 0;
}



void setExtruderPosition(int32_t position)
{
  extruderStepper.setCurrentPosition(position);
}



void setHeaterFanSpeed(uint8_t newFanSpeed)
{
  heaterFanSpeed = newFanSpeed;
  analogWrite(FDM_HEATER_FAN_PIN, heaterFanSpeed);
}



uint8_t getHeaterFanSpeed()
{
  return heaterFanSpeed;
}



void setFilamentFanSpeed(uint8_t newFanSpeed)
{
  filamentFanSpeed = newFanSpeed;
  analogWrite(FDM_FILAMENT_FAN_PIN, filamentFanSpeed);
}



uint8_t getFilamentFanSpeed()
{
  return filamentFanSpeed;
}



void setHeaterGoal(uint8_t newHeaterGoal)
{
  heaterGoal = newHeaterGoal;
}



double getHeatertempp()
{
  return analogRead(HEATER_SWEEP_SENSOR_PIN) * 0.05;
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


void setBedGoal(uint8_t newBedGoal)
{
  bedGoal = newBedGoal;
}



double getBedTemp()
{
  return analogRead(HEATER_SWEEP_SENSOR_PIN) * 0.05;
}


void updateBed()
{
  float heatData = analogRead(HEATER_SWEEP_SENSOR_PIN);
  heatData *= 0.05;

  if(heatData < bedGoal)
  {
    digitalWrite(HEATER_BED_PIN, 1);
  }
  else
  {
    digitalWrite(HEATER_BED_PIN, 0);
  }
}

#endif
