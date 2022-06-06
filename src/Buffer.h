#ifndef BUFFER_H
#define BUFFER_H

#include "stdint.h"

uint16_t getEmptySlots();
int32_t* getBufferCoordinates();
uint64_t* getNozzleState();
void buffer_update();
void buffer_next(uint8_t isSDCard);
void buffer_look_ahead();
void buffer_current();
void buffer_fill(uint8_t isSDCard);
uint8_t getBufferEmpty();

#endif
