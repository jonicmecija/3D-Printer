#ifndef USBSOURCE_H
#define USBSOURCE_H

#include "stdint.h"

uint8_t isEndOfCommandList();
uint8_t getUSBCharacter();
void resetCommandList();
void returnOkSignal();

#endif
