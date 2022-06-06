#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

#include "stdint.h"

void startSDPrint(uint8_t isDryPrint, char* fileName);
void controlLoop();
void stopSDPrint();
uint8_t isPrinting();

#endif
