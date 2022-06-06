//--------------------LIBRARY INCLUDES--------------------//
#include "GCodeParser.h"
#include "Buffer.h"
#include "SDSource.h"
#include "USBSource.h"
#include "Arduino.h"
#include "Nozzle.h"


//--------------------DEFINES--------------------//
#define BUFFER_SIZE       120
#define MAX_OFFSET        20


//--------------------VARIABLES--------------------//
static int32_t buffer_coordinate_slot[7][BUFFER_SIZE];
static int32_t buffer_coordinate_read[7];
static uint64_t buffer_nozzle_read[2];

#ifdef C6602A
static uint16_t buffer_nozzle_slot[2][BUFFER_SIZE];
#else
static uint64_t buffer_nozzle_slot[2][BUFFER_SIZE];
#endif

static uint32_t read_line = 0;
static uint32_t write_line = 0;
static uint16_t read_OFFSET = 0;

static uint16_t empty_slots = 0;
//--------------------GETTER FUNCTIONS--------------------//
/*! \brief Returns 1 if the buffer is empty, returns 0 otherwise
 * 
 */
uint8_t getBufferEmpty()
{
  return empty_slots == BUFFER_SIZE;
}


/*! \brief Returns the number of empty slots in the command buffer
 * 
 */
uint16_t getEmptySlots()
{
  return empty_slots;
}


/*! \brief Returns the coordinate array from the latest buffer slot 
 * 
 */
int32_t* getBufferCoordinates()
{
  return buffer_coordinate_read;
}


/*! \brief Returns the nozzle state array from the latest buffer slot
 * 
 *  The nozzle state array is an array of 2 uint64_t which each bit in
 *  each nozzle is the state of 1 of the 128 nozzles for the xaar128
 */
uint64_t* getNozzleState()
{
  return buffer_nozzle_read;
}

//--------------------FUNCTIONS--------------------//
/*! \brief Adds up to one command to the buffer and updates the buffer
 *  state variables
 *  
 *  This function only can fill 1 buffer slot at a time, to fill the buffer
 *  call this function in a loop while polling getEmptySlots()
 */
void buffer_update()
{
  if (empty_slots > 0)
  {

    if (isEndOfFile() == 0)
    {
      updateSDCard(1);
    }
    
    if (isNewLine() == 1)
    {
      clearNewLine();


      for (int w = 0; w < 7; w++)
      {
        buffer_coordinate_slot[w][write_line] = getExportBuffer()[w];
      }


      for(int index = 0; index < 2; index++)
      {
        buffer_nozzle_slot[index][write_line] = getNozzleBuffer()[index];
      }    


      write_line ++;
      if (write_line >= BUFFER_SIZE)
      {
        write_line = 0;
      }


      empty_slots --;
    }
  }
}


/*! \brief Selects the next item in the command buffer
 * 
 *  This fucntion also updates the end of print variable
 */
void buffer_next(uint8_t isSDCard)
{
  read_line++;
  if (read_line >= BUFFER_SIZE)
  {
    read_line = 0;
  }


  for (int w = 0; w < 7; w++)
  {
    buffer_coordinate_read[w] = buffer_coordinate_slot[w][read_line];
  }


  for(int index = 0; index < 2; index++)
  {
    buffer_nozzle_read[index] = buffer_nozzle_slot[index][read_line];
  }


  if ((!isEndOfFile() && isSDCard ) || !isSDCard)
  {
    empty_slots ++;
  }
  
  read_OFFSET = 0;
}


/*! \brief This function updates the buffer to look at the slot ahead without modifiying
 *         any buffer state variables
 *         
 *  The function places the buffer result into the buffer read variables however it does not
 *  change the buffer state variables. Before calling any other buffer function, call
 *  buffer_current() to return the buffer to it's original state
 */
void buffer_look_ahead() //looks ahead one slot to look for additional coordinates, can be stacked
{
  if (read_OFFSET < MAX_OFFSET)
  { 
    read_OFFSET ++;
  }


  uint32_t readtemp = read_line;
  readtemp += read_OFFSET;


  if (readtemp >= BUFFER_SIZE)
  {
    readtemp = 0;
  }


  if (isEndOfFile() && (readtemp == write_line))
  {
    read_OFFSET--; //step back one step
    readtemp = read_line;
    readtemp += read_OFFSET;

    if (readtemp >= BUFFER_SIZE)
    {
      readtemp = 0;
    }

  }

  
  else
  {
    for (int w = 0; w < 7; w++)
    {
      buffer_coordinate_read[w] = buffer_coordinate_slot[w][readtemp];
    }
    for(int index = 0; index < 2; index++){
      buffer_nozzle_read[index] = buffer_nozzle_slot[index][readtemp];
    }  
  }    
}


/*! \brief This function returns the buffer back to the original state
 *         after a call to buffer_look_ahead
 */
void buffer_current() //go back to current buffer slot
{
  read_OFFSET = 0;


  for (int w = 0; w < 7; w++)
  {
    buffer_coordinate_read[w] = buffer_coordinate_slot[w][read_line];
  }

  
  for(int index = 0; index < 2; index++){
    buffer_nozzle_read[index] = buffer_nozzle_slot[index][read_line];
  }
}


/*! \brief This function fills the buffer and initializes buffer variables
 * 
 *  This function should only be called at the beginning of the print,
 *  otherwise the the buffer state variables will be corrupted
 */
void buffer_fill(uint8_t isSDCard) //fills entire buffer (used before printing, not any other place!)
{
  write_line = 0;
  read_line = 0;
  empty_slots = 0;
  read_OFFSET = 0;


  while (write_line < BUFFER_SIZE && !((isEndOfFile() && isSDCard) || (isEndOfCommandList() && !isSDCard)))
  { 
    updateSDCard(isSDCard);
    if (isNewLine() == 1)
    {
      clearNewLine();

      for (int w = 0; w < 7; w++)
      {
        buffer_coordinate_slot[w][write_line] = getExportBuffer()[w];
      }
      for(int index = 0; index < 2; index++){
        buffer_nozzle_slot[index][write_line] = getNozzleBuffer()[index];
      }

      write_line++;
    }

  }


  if(!isSDCard)
  {
    empty_slots = BUFFER_SIZE - (write_line - 1);
  }
  
  
  if (write_line >= BUFFER_SIZE)
  {
    write_line = 0;
  }


  //put first command in read line
  for (int w = 0; w < 7; w++)
  {
    buffer_coordinate_read[w] = buffer_coordinate_slot[w][read_line];
  }


  for(int index = 0; index < 2; index++){
    buffer_nozzle_read[index] = buffer_nozzle_slot[index][read_line];
  }
}
