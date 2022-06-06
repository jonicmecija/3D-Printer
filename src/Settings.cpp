#include "Settings.h"
#include <EEPROM.h>
#include <Arduino.h>

#define TRAVEL_SPEED                        0
#define TRAVEL_ACCELERATON                  2

#define NOZZLE_SWEEP_SPEED                  4
#define NOZZLE_SWEEP_ACCEL                  6
#define NOZZLE_DOUBLE_DIRECTION_OFFSET      8
#define NOZZLE_DPI                          10

#define SINGLE_NOZZLE_SWEEP_SPEED           12
#define SINGLE_NOZZLE_SWEEP_ACCEL           14
#define SINGLE_NOZZLE_SWEEP_NOZZLE          16

#define HEATER_TRAVEL_SPEED                 18

#define X_STEPS_PER_MM                      20
#define Y_STEPS_PER_MM                      22
#define Z_STEPS_PER_MM                      24
#define S_STEPS_PER_MM                      26
#define B_STEPS_PER_MM                      28
#define F_STEPS_PER_MM                      30

#define S_OFFSET                            32
#define X_OFFSET                            36
#define Y_OFFSET                            40
#define Z_OFFSET                            44
#define F_OFFSET                            48
#define B_OFFSET                            52

#define S_MOTOR_SPEED                       54      
#define S_MOTOR_ACCEL                       56
#define F_MOTOR_SPEED                       58
#define F_MOTOR_ACCEL                       60
#define B_MOTOR_SPEED                       62
#define B_MOTOR_ACCEL                       64

#define Z_LIFT_HEIGHT                       66
#define S_TRAVEL_DISTANCE                   68

#define S_HOME_SPEED                        70
#define S_HOME_BACKSTEP                     72
#define Y_EXTRA_MOVEMENT                    74
#define G_HOME_SPEED                        76
#define G_HOME_BACKSTEP                     78
#define F_HOME_SPEED                        80
#define F_HOME_BACKSTEP                     82
#define B_HOME_SPEED                        84
#define B_HOME_BACKSTEP                     86
#define MAGIC_NUMBER_ADDRESS                88
#define MAGIC_NUMBER_VALUE                  200
#define ARC_INCREMENT_AMOUNT                92

#define EXTRUDER_MOTOR_SPEED                96
#define EXTRUDER_MOTOR_ACCEL                98
#define EXTRUDER_STEPS_PER_MM               100


void readEEPROM();
void writeEEPROM();


uint16_t optTravelSpeed = 60 * 20;
uint16_t optTravelAccel = 5000;

uint16_t optNozzleSweepSpeed = 25 * 20;
uint16_t optNozzleSweepAccel = 1000;
uint16_t optNozzleDoubleDirOffset = 2;
uint16_t optNozzleDPI = 96;

uint16_t optSingleNozzleSweepSpeed = 25 * 20;
uint16_t optSingleNozzleSweepAccel = 1000;
uint16_t optSingleNozzleSweepNozzle = 0;

uint16_t optHeaterTravelSpeed = 20 * 20;

uint16_t optXStepsPermm = 20;
uint16_t optYStepsPermm = 20;
uint16_t optZStepsPermm = 20;
uint16_t optSStepsPermm = 20;
uint16_t optBStepsPermm = 20;
uint16_t optFStepsPermm = 20;

int32_t optSOffset = 0;
int32_t optXOffset = -3500;
int32_t optYOffset = -2000;
int32_t optZOffset = 0;
int32_t optFOffset = 0;
int32_t optBOffset = 0;

uint16_t optSMotorSpeed = 40 * 20;      
uint16_t optSMotorAccel = 1000;
uint16_t optFMotorSpeed = 30 * 20;
uint16_t optFMotorAccel = 1000;
uint16_t optBMotorSpeed = 30 * 20;
uint16_t optBMotorAccel = 1000;

uint16_t optZLiftHeight = 25 * 20;
uint16_t optSTravelDistance = 300;

uint16_t optSHomeSpeed = 60 * 20;
uint16_t optSHomeBackstep = 50;
uint16_t optGHomeSpeed = 60 * 20;
uint16_t optGHomeBackstep = 50;
uint16_t optFHomeSpeed = 60 * 20;
uint16_t optFHomeBackstep = 50;
uint16_t optBHomeSpeed = 60 * 20;
uint16_t optBHomeBackstep = 50;
uint16_t optYExtraMovement = 1;

double optArcIncrementAmount = 0.1;

uint32_t magicNumberTest = 0;

uint16_t optExtruderMotorSpeed = 100;
uint16_t optExtruderMotorAccel = 1000;
uint16_t optExtruderStepsPermm = 20;


void initializeSettings()
{
  uint32_t magicNumberTest = 0;
  EEPROM.get(MAGIC_NUMBER_ADDRESS, magicNumberTest);

  if(magicNumberTest == MAGIC_NUMBER_VALUE)
  {
    readEEPROM();
  }else{
    writeEEPROM();
  }
}



void readEEPROM()
{
  EEPROM.get(TRAVEL_SPEED, optTravelSpeed);
  EEPROM.get(TRAVEL_ACCELERATON, optTravelAccel);
  
  EEPROM.get(NOZZLE_SWEEP_SPEED, optNozzleSweepSpeed);
  EEPROM.get(NOZZLE_SWEEP_ACCEL, optNozzleSweepAccel);
  EEPROM.get(NOZZLE_DOUBLE_DIRECTION_OFFSET, optNozzleDoubleDirOffset);
  EEPROM.get(NOZZLE_DPI, optNozzleDPI);
  
  EEPROM.get(SINGLE_NOZZLE_SWEEP_SPEED, optSingleNozzleSweepSpeed);
  EEPROM.get(SINGLE_NOZZLE_SWEEP_ACCEL, optSingleNozzleSweepAccel);
  EEPROM.get(SINGLE_NOZZLE_SWEEP_NOZZLE, optSingleNozzleSweepNozzle);
  
  EEPROM.get(HEATER_TRAVEL_SPEED, optHeaterTravelSpeed);
  
  EEPROM.get(X_STEPS_PER_MM, optXStepsPermm);
  EEPROM.get(Y_STEPS_PER_MM, optYStepsPermm);
  EEPROM.get(Z_STEPS_PER_MM, optZStepsPermm);
  EEPROM.get(S_STEPS_PER_MM, optSStepsPermm);
  EEPROM.get(B_STEPS_PER_MM, optBStepsPermm);
  EEPROM.get(F_STEPS_PER_MM, optFStepsPermm);
  
  EEPROM.get(S_OFFSET, optSOffset);
  EEPROM.get(X_OFFSET, optXOffset);
  EEPROM.get(Y_OFFSET, optYOffset);
  EEPROM.get(Z_OFFSET, optZOffset);
  EEPROM.get(F_OFFSET, optFOffset);
  EEPROM.get(B_OFFSET, optBOffset);
  
  EEPROM.get(S_MOTOR_SPEED, optSMotorSpeed);
  EEPROM.get(S_MOTOR_ACCEL, optSMotorAccel);
  EEPROM.get(F_MOTOR_SPEED, optFMotorSpeed);
  EEPROM.get(F_MOTOR_ACCEL, optFMotorAccel);
  EEPROM.get(B_MOTOR_SPEED, optBMotorSpeed);
  EEPROM.get(B_MOTOR_ACCEL, optBMotorAccel);
  
  EEPROM.get(Z_LIFT_HEIGHT, optZLiftHeight);
  EEPROM.get(S_TRAVEL_DISTANCE, optSTravelDistance);
  
  EEPROM.get(S_HOME_SPEED, optSHomeSpeed);
  EEPROM.get(S_HOME_BACKSTEP, optSHomeBackstep);
  EEPROM.get(G_HOME_SPEED, optGHomeSpeed);
  EEPROM.get(G_HOME_BACKSTEP, optGHomeBackstep);
  EEPROM.get(F_HOME_SPEED, optFHomeSpeed);
  EEPROM.get(F_HOME_BACKSTEP, optFHomeBackstep);
  EEPROM.get(B_HOME_SPEED, optBHomeSpeed);
  EEPROM.get(B_HOME_BACKSTEP, optBHomeBackstep);
  
  EEPROM.get(Y_EXTRA_MOVEMENT, optYExtraMovement);

  EEPROM.get(ARC_INCREMENT_AMOUNT, optArcIncrementAmount);

  EEPROM.get(EXTRUDER_MOTOR_SPEED, optExtruderMotorSpeed);
  EEPROM.get(EXTRUDER_MOTOR_ACCEL, optExtruderMotorAccel);
  EEPROM.get(EXTRUDER_STEPS_PER_MM, optExtruderStepsPermm);
}



void writeEEPROM()
{
  EEPROM.put(TRAVEL_SPEED, optTravelSpeed);
  EEPROM.put(TRAVEL_ACCELERATON, optTravelAccel);
  
  EEPROM.put(NOZZLE_SWEEP_SPEED, optNozzleSweepSpeed);
  EEPROM.put(NOZZLE_SWEEP_ACCEL, optNozzleSweepAccel);
  EEPROM.put(NOZZLE_DOUBLE_DIRECTION_OFFSET, optNozzleDoubleDirOffset);
  EEPROM.put(NOZZLE_DPI, optNozzleDPI);
  
  EEPROM.put(SINGLE_NOZZLE_SWEEP_SPEED, optSingleNozzleSweepSpeed);
  EEPROM.put(SINGLE_NOZZLE_SWEEP_ACCEL, optSingleNozzleSweepAccel);
  EEPROM.put(SINGLE_NOZZLE_SWEEP_NOZZLE, optSingleNozzleSweepNozzle);
  
  EEPROM.put(HEATER_TRAVEL_SPEED, optHeaterTravelSpeed);
  
  EEPROM.put(X_STEPS_PER_MM, optXStepsPermm);
  EEPROM.put(Y_STEPS_PER_MM, optYStepsPermm);
  EEPROM.put(Z_STEPS_PER_MM, optZStepsPermm);
  EEPROM.put(S_STEPS_PER_MM, optSStepsPermm);
  EEPROM.put(B_STEPS_PER_MM, optBStepsPermm);
  EEPROM.put(F_STEPS_PER_MM, optFStepsPermm);
  
  EEPROM.put(S_OFFSET, optSOffset);
  EEPROM.put(X_OFFSET, optXOffset);
  EEPROM.put(Y_OFFSET, optYOffset);
  EEPROM.put(Z_OFFSET, optZOffset);
  EEPROM.put(F_OFFSET, optFOffset);
  EEPROM.put(B_OFFSET, optBOffset);
  
  EEPROM.put(S_MOTOR_SPEED, optSMotorSpeed);
  EEPROM.put(S_MOTOR_ACCEL, optSMotorAccel);
  EEPROM.put(F_MOTOR_SPEED, optFMotorSpeed);
  EEPROM.put(F_MOTOR_ACCEL, optFMotorAccel);
  EEPROM.put(B_MOTOR_SPEED, optBMotorSpeed);
  EEPROM.put(B_MOTOR_ACCEL, optBMotorAccel);
  
  EEPROM.put(Z_LIFT_HEIGHT, optZLiftHeight);
  EEPROM.put(S_TRAVEL_DISTANCE, optSTravelDistance);
  
  EEPROM.put(S_HOME_SPEED, optSHomeSpeed);
  EEPROM.put(S_HOME_BACKSTEP, optSHomeBackstep);
  EEPROM.put(G_HOME_SPEED, optGHomeSpeed);
  EEPROM.put(G_HOME_BACKSTEP, optGHomeBackstep);
  EEPROM.put(F_HOME_SPEED, optFHomeSpeed);
  EEPROM.put(F_HOME_BACKSTEP, optFHomeBackstep);
  EEPROM.put(B_HOME_SPEED, optBHomeSpeed);
  EEPROM.put(B_HOME_BACKSTEP, optBHomeBackstep);
  
  EEPROM.put(Y_EXTRA_MOVEMENT, optYExtraMovement);

  EEPROM.put(ARC_INCREMENT_AMOUNT, optArcIncrementAmount);

  uint32_t magicNumberTest = MAGIC_NUMBER_VALUE;
  EEPROM.put(MAGIC_NUMBER_ADDRESS, magicNumberTest);

  EEPROM.put(EXTRUDER_MOTOR_SPEED, optExtruderMotorSpeed);
  EEPROM.put(EXTRUDER_MOTOR_ACCEL, optExtruderMotorAccel);
  EEPROM.put(EXTRUDER_STEPS_PER_MM, optExtruderStepsPermm);
}
