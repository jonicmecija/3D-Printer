//--------------------LIBRARY INCLUDES--------------------//
#include "Arduino.h"
#include "Input.h"
#include "Pins.h"


//--------------------VARIABLES--------------------//
static uint8_t currentInputByte;
static uint8_t previousEncoderState;


//--------------------FUNCTIONS--------------------//
/*! \brief Initializes the pins and states required for
 *         the inputs to work
 */
void initInputs()
{
  pinMode(INPUT_LATCH_PIN, OUTPUT);
  pinMode(INPUT_CLOCK_PIN, OUTPUT); 
  pinMode(INPUT_DATA_PIN, INPUT);

  pinMode(INPUT_ENCODER_A_PIN, INPUT);
  pinMode(INPUT_ENCODER_B_PIN, INPUT);
}


/*! \brief Updates the input holding variables to the
 *         current state
 *         
 *         Call this function as often as possible
 */
void updateInputs()
{
  //Send a pulse to load data into the shift register
  digitalWrite(INPUT_LATCH_PIN, LOW);
  digitalWrite(INPUT_LATCH_PIN, HIGH);

  for(uint8_t currentBit = 0; currentBit < 8; currentBit++)
  {
    uint8_t currentBitValue = !digitalRead(INPUT_DATA_PIN);
    
    if(currentBitValue)
    {
      currentInputByte |= 1 << currentBit;
    }else{
      currentInputByte &= ~(1 << currentBit);
    }

    //Clock more data in
    digitalWrite(INPUT_CLOCK_PIN, HIGH);
    digitalWrite(INPUT_CLOCK_PIN, LOW); 
  }
}


/*! \brief Takes a defined button and returns
 *         true if the button is pressed and
 *         false otherwise
 */
uint8_t getButton(uint8_t button)
{
  return (currentInputByte >> button) & 1;
}


/*! \brief Returns a encoder direction definition
 *         depending on which direction the encoder
 *         is being turned
 */
uint8_t getEncoderDirection()
{
  uint8_t higherEncoderState;;
  uint8_t lowerEncoderState;
  
  uint8_t encoderA = digitalRead(INPUT_ENCODER_A_PIN);
  uint8_t encoderB = digitalRead(INPUT_ENCODER_B_PIN);

  //Conbert from grey code to binary
  uint8_t encoderState = (encoderB << 1) & (encoderA ^ encoderB);
  
  if(previousEncoderState == 4)
  {
    higherEncoderState = 0;
    lowerEncoderState = previousEncoderState - 1;
  }
  else if(previousEncoderState == 0)
  {
    higherEncoderState = previousEncoderState + 1;
    lowerEncoderState = 4;
  }
  else
  {
    higherEncoderState = previousEncoderState + 1;
    lowerEncoderState = previousEncoderState - 1;
  }

  previousEncoderState = encoderState;

  if(encoderState == higherEncoderState)
  {
    return ENCODER_LEFT;
  }
  else if(encoderState == lowerEncoderState)
  {
    return ENCODER_RIGHT;
  }
  else
  {
    return ENCODER_STOPPED;
  }
}
