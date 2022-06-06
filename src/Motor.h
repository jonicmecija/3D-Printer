#ifndef MOTOR_H
#define MOTOR_H

#define X_MOTOR  0
#define Y_MOTOR  1
#define Z_MOTOR  2
#define F_MOTOR  3
#define S_MOTOR  4
#define B_MOTOR  5
#define G_MOTORS 6

void moveArc(int32_t I_10um, int32_t J_10um, uint8_t dir, int32_t feedrate);
void disableMotors();
int32_t getMotorPosition(uint8_t axis);
void homeGantry();
void initMotors();
void updateGantry();
void setGantryTarget(int32_t X_10um, int32_t Y_10um, int32_t Z_10um, int32_t feedrate);
void startSweepX(uint8_t X_start_dir);
void stopSweepX();
void enableMotors();
void newLayer(int NL_X_target, int NL_Y_target, int layer_thickness);
void setOutOfFeedMaterial(uint8_t newValue);
uint8_t getTargetReached();
void setPrintingMode(uint8_t mode);
uint8_t getPrintingMode();
uint8_t getXConstantMovement();
void homeMotor(uint8_t axis);
void setMotorPosition(uint8_t axis, int32_t newValue);


#endif
