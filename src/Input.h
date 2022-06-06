#ifndef INPUT_H
#define INPUT_H


#include "Stdint.h"


#define BUTTON_MENU1 2
#define BUTTON_MENU2 1
#define BUTTON_MENU3 0
#define BUTTON_UP    6
#define BUTTON_DOWN  3
#define BUTTON_LEFT  7
#define BUTTON_RIGHT 4
#define BUTTON_ENTER 5
#define ENCODER_LEFT 0
#define ENCODER_STOPPED 1
#define ENCODER_RIGHT 2



void updateInputs();
uint8_t getButton(uint8_t button);
uint8_t getEncoderDirection();
void initInputs();


#endif
