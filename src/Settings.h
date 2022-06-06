#ifndef SETTINGS_H
#define SETTINGS_H


#include "stdint.h"


#define DETERMINISTIC_MOTORS

//#define XAAR128
#define C6602A
//#define TTL_LASER
//#define MILL
//#define FDM


extern uint16_t optTravelSpeed;
extern uint16_t optTravelAccel;

extern uint16_t optNozzleSweepSpeed;
extern uint16_t optNozzleSweepAccel;
extern uint16_t optNozzleDoubleDirOffset;
extern uint16_t optNozzleDPI;

extern uint16_t optSingleNozzleSweepSpeed;
extern uint16_t optSingleNozzleSweepAccel;
extern uint16_t optSingleNozzleSweepNozzle;

extern uint16_t optHeaterTravelSpeed;

extern uint16_t optXStepsPermm;
extern uint16_t optYStepsPermm;
extern uint16_t optZStepsPermm;
extern uint16_t optSStepsPermm;
extern uint16_t optBStepsPermm;
extern uint16_t optFStepsPermm;

extern int32_t optSOffset;
extern int32_t optXOffset;
extern int32_t optYOffset;
extern int32_t optZOffset;
extern int32_t optFOffset;
extern int32_t optBOffset;

extern uint16_t optSMotorSpeed;      
extern uint16_t optSMotorAccel;
extern uint16_t optFMotorSpeed;
extern uint16_t optFMotorAccel;
extern uint16_t optBMotorSpeed;
extern uint16_t optBMotorAccel;

extern uint16_t optZLiftHeight;
extern uint16_t optSTravelDistance ;

extern uint16_t optSHomeSpeed;
extern uint16_t optSHomeBackstep;
extern uint16_t optGHomeSpeed;
extern uint16_t optGHomeBackstep;
extern uint16_t optFHomeSpeed;
extern uint16_t optFHomeBackstep;
extern uint16_t optBHomeSpeed;
extern uint16_t optBHomeBackstep;
extern uint16_t optYExtraMovement;

extern double optArcIncrementAmount;

extern uint16_t optExtruderMotorSpeed;
extern uint16_t optExtruderMotorAccel;
extern uint16_t optExtruderStepsPermm;

void initializeSettings();

#endif
