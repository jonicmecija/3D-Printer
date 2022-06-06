#ifndef GCODE_H
#define GCODE_H

extern uint8_t isInM28Mode;

uint8_t updateG0();
uint8_t updateG1();
void updateG2();
void updateG3();
uint32_t updateG4();
void updateG20();
void updateG21();
void updateG28();
void updateG90();
void updateG91();
void updateG92();
uint8_t updateM0(uint8_t isPrintPaused);
void updateM3();
void updateM4();
void updateM5();
void updateM20();
void updateM23();
void updateM28();
void clearM28Mode();
void updateM92();
void updateM104();
void updateM105();
void updateM106();
void updateM107();
uint8_t updateM109();
uint8_t updateM190();

void checkAxisParamaters(int32_t* newValue, uint8_t motorAxis);

#endif
