#include "Arduino.h"
#include <LiquidCrystal.h>
#include "Menu.h"
#include "Input.h"
#include "Pins.h"
#include "SDSource.h"
#include "Motor.h"
#include "ControlLoop.h"


#define MAIN_MENU 0
#define MOTOR 1
#define SETTINGS 2
#define PRINT_SELECT 3
#define PRINT 4
#define HOME 5
#define MOVE 6
#define SPEED 7
#define ACCELERATION 8
#define DISTANCE 9
#define X_MENU 10
#define Y_MENU 11
#define Z_MENU 12
#define F_MENU 13
#define B_MENU 14
#define S_MENU 15
#define G_MENU 16
#define T_MENU 17
#define S_MENU 18
#define DPI_MENU 19
#define PRINT_DRY 20
#define PRINT_WET 21


char* mainMenuOptions[] = {"Print", "Motor", "Settings"};
uint8_t mainMenuTransfers[] = {PRINT_SELECT, MOTOR, SETTINGS};


char* printSelectMenuOptions[] = {"Dry Run", "Wet Run"};
uint8_t printSelectMenuTransfers[] = {PRINT_DRY, PRINT_WET};


char* motorMenuOptions[] = {"Home", "Move"};
uint8_t motorMenuTransfers[] = {HOME, MOVE};


char* settingsMenuOptions[] = {"Speed", "Acceleration", "Distance", "Print"};
uint8_t settingsMenuTransfers[] = {SPEED, ACCELERATION, DISTANCE, PRINT};


char* moveMenuOptions[] = {"Move X", "Move Y", "Move Z", "Move Feed Tank", "Move Build Tank", "Move Spreader"};
uint8_t moveMenuTransfers[] = {X_MENU, Y_MENU, Z_MENU, F_MENU, B_MENU, S_MENU};


char* homeMenuOptions[] = {"Home Gantry", "Home Tanks", "Home Spreader"};
uint8_t motorSystemMenuTransfers[] = {G_MENU, T_MENU, S_MENU};


char* speedMenuOptions[] = {"Gantry Speed", "Tank Speed", "Spreader Speed"};


char* accelMenuOptions[] = {"Gantry Accel", "Tank Accel", "Spreader Accel"};


char* distanceMenuOptions[] = {"Spreader Dist"};
uint8_t distanceMenuTransfers[] = {S_MENU};


char* printSettingsMenuOptions[] = {"DPI"};
uint8_t dpiMenuTransfers[] = {DPI_MENU};


char* printMenuOptions[30];
char printMenuOptionsValues[30][20];



static LiquidCrystal lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

uint8_t menuState = MAIN_MENU;
uint8_t prevMenuState = -1;
uint8_t selectedItemIndex = 0;
uint8_t prevSelectedItemIndex= 0;

uint8_t downButtonHeld = 0;
uint8_t upButtonHeld = 0;
uint8_t menuButtonHeld = 0;
uint8_t enterButtonHeld = 0;

uint8_t numberFiles = 0;
uint8_t isDryPrint = 0;
uint8_t updateSDMenu = 1;



void updateScreenInputs(uint8_t* tranferIndex);
void showSelectionMenu(char** menuOptions, uint8_t menuLength);
void updateScreenInputs();
uint8_t getFiles();
void checkHomingStatus();




void initMenu(){
  lcd.begin(20, 4);
}



void showMenu()
{
  switch(menuState)
  {
    case MAIN_MENU:
      updateSDMenu = 1;
      showSelectionMenu(mainMenuOptions, 3);
      updateScreenInputs(mainMenuTransfers);    
    break;

    case PRINT_SELECT:
      showSelectionMenu(printSelectMenuOptions, 2);
      updateScreenInputs(printSelectMenuTransfers); 
    break;

    case PRINT_WET:
      isDryPrint = 0;
      menuState = PRINT;
    break;

    case PRINT_DRY:
      isDryPrint = 1;
      menuState = PRINT;
    break;
    
    case PRINT:  
      if(updateSDMenu)
      {
        if(getFiles()){break;}
        updateSDMenu = 0;
      }
      showSelectionMenu(printMenuOptions, numberFiles);
      updateScreenInputs(mainMenuTransfers);
    break;

    case MOTOR:
      showSelectionMenu(motorMenuOptions, 2);
      updateScreenInputs(motorMenuTransfers);
    break;

    case HOME:
      showSelectionMenu(homeMenuOptions, 3);
      updateScreenInputs(motorSystemMenuTransfers);
      checkHomingStatus();
    break;

    case MOVE: 
      showSelectionMenu(moveMenuOptions, 6);
      updateScreenInputs(moveMenuTransfers);
    break;
    
    case SETTINGS: 
      showSelectionMenu(settingsMenuOptions, 4);
      updateScreenInputs(settingsMenuTransfers);
    break;

    case SPEED:
      showSelectionMenu(speedMenuOptions, 3);
      updateScreenInputs(motorSystemMenuTransfers);      
    break;
    
    case ACCELERATION:
      showSelectionMenu(accelMenuOptions, 3);
      updateScreenInputs(motorSystemMenuTransfers);
      break;
      
    case DISTANCE: 
      showSelectionMenu(distanceMenuOptions, 1);
      updateScreenInputs(distanceMenuTransfers);
      break;
  }
}



void checkHomingStatus()
{
  switch(menuState)
  {
    case G_MENU:
      homeMotor(G_MOTORS);
      menuState = MAIN_MENU;
    break;
    case T_MENU:
      homeMotor(F_MOTOR);
      homeMotor(B_MOTOR);
      menuState = MAIN_MENU;
    break;
    case S_MENU:
      homeMotor(S_MOTOR);
      menuState = MAIN_MENU;
    break;
    default:
    break;
  }
}



uint8_t getFiles()
{
  numberFiles = 0;

  for(int index = 0; index < 30; index++)
  {
    printMenuOptions[index] = printMenuOptionsValues[index];
  }

  if(prevMenuState != menuState)
  {
    startFileNameList();
    while(!getNextFileName(printMenuOptions[numberFiles]))
    {
      numberFiles++;
    }
    endFileNameList();
  }
  
  if(numberFiles == 0)
  {
    prevMenuState = menuState;
    menuState = MAIN_MENU;
    return 1;
  }
  
  return 0;
}



void updateScreenInputs(uint8_t* transferIndex)
{
  updateInputs();

  if(getButton(BUTTON_MENU1) && !menuButtonHeld)
  {
    menuButtonHeld = 1;
    prevMenuState = menuState;
    menuState = MAIN_MENU;
  }
  
  else if(getButton(BUTTON_ENTER) && !enterButtonHeld)
  {
    prevMenuState = menuState;
    if(menuState == PRINT)
    {
      startSDPrint(isDryPrint, printMenuOptions[selectedItemIndex]);
      menuState = MAIN_MENU;
    }else{
      menuState = transferIndex[selectedItemIndex];
    }
    
    enterButtonHeld = 1;
  }

  else if(getButton(BUTTON_UP) && !upButtonHeld)
  {
    upButtonHeld = 1;
    selectedItemIndex++;
  }
  
  else if(getButton(BUTTON_DOWN) && !downButtonHeld)
  {
    downButtonHeld = 1;
    if(selectedItemIndex != 0){
      selectedItemIndex--;
    }
  } 

  if(!getButton(BUTTON_MENU1) && menuButtonHeld)
  {
    menuButtonHeld = 0;
  }
  
  if(!getButton(BUTTON_ENTER) && enterButtonHeld)
  {
    enterButtonHeld = 0;
  }

  if(!getButton(BUTTON_UP) && upButtonHeld)
  {
    upButtonHeld = 0;
  }
  
  if(!getButton(BUTTON_DOWN) && downButtonHeld)
  {
    downButtonHeld = 0;
  } 
}



void showSelectionMenu(char** menuOptions, uint8_t menuLength)
{
  if(prevMenuState != menuState)
  {
    prevSelectedItemIndex = 1;
    selectedItemIndex = 0;
  }

  if(selectedItemIndex < menuLength)
  {
    if(prevSelectedItemIndex != selectedItemIndex)
    { 
      uint8_t menuIndexLowBounds = selectedItemIndex - 3;
      if(selectedItemIndex < 3)
      {
        menuIndexLowBounds = 0;
      }
      
  
      if(prevSelectedItemIndex > 4 || selectedItemIndex > 4 || prevMenuState != menuState)
      {
        if(selectedItemIndex < 4)
        {
          menuIndexLowBounds = 0;
        }
    
    
        uint8_t linesToPrint = menuLength - menuIndexLowBounds;
        if(linesToPrint > 4)
        {
          linesToPrint = 4;
        }

        
        lcd.clear();
        for(uint8_t line = 0; line < linesToPrint; line++)
        {
          lcd.setCursor(0, line);
          lcd.print(" ");
          lcd.print(menuOptions[menuIndexLowBounds + line]);  
        }
      }
      

      lcd.setCursor(0, prevSelectedItemIndex - menuIndexLowBounds);
      lcd.print(" ");
      lcd.setCursor(0, selectedItemIndex - menuIndexLowBounds);
      lcd.print("*");
  
      if(prevMenuState != menuState)
      {
        prevMenuState = menuState;
      }
  
      prevSelectedItemIndex = selectedItemIndex;
    }
  }
  else
  {
    selectedItemIndex = prevSelectedItemIndex;
  }
}
  
