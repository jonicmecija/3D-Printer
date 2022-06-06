#include "Input.h"
#include "Motor.h"
#include "GCodeParser.h"
#include "Menu.h"
#include "Nozzle.h"
#include "Settings.h"
#include "ControlLoop.h"
#include "SDSource.h"
#include "USBSource.h"
#include "Arduino.h"

uint32_t time_goal = 0;

void setup() {
  Serial.begin(115200);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  initSDSource();
  initInputs();
  initSDCard();
  initNozzle();
  initMenu();
  initMotors();
  initializeSettings();
  returnOkSignal();
}

uint16_t counter = 0;
void loop() {
  if((millis() > time_goal) && !isPrinting())
  {
    time_goal = millis() + 17;
    showMenu();
  } 
  if(counter > 1000)
    {
      Serial.println("good");
      counter = 0;
    }else{
      counter++;
    }
  controlLoop();
}
