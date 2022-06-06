//--------------------LIBRARY INCLUDES--------------------//
#include <Arduino.h>
#include "GCodeParser.h"
#include "SDSource.h"
#include "USBSource.h"
#include <Math.h>


//--------------------DEFINES--------------------//
#define NOZZLE_COUNT 12 //amount of nozzles in cartridge


//--------------------VARIABLES--------------------//
static uint64_t nozzle_history[2] = {}; //what nozzles were on the previous line
static uint64_t nozzle_stated[2] = {}; //what nozzles are being changed

static uint8_t block_pos = 0; //where in the block is being read
static int8_t hasDecimal = -1; //where to stop on the X, Y and Z numerators (used after a comma)

static uint8_t temp_block[7]; //temporary storage for single characters
static int32_t temp_buffer[7]; //temporary line buffer

static uint8_t line_pos = 0; //where in the line is being read (X,Y,Z, identifiers, nozzles, direction ie.) 
static uint8_t line_code = 0; //detemines if a line is a nozzle code or a move code, 0 is undetermined, 1 is nozzle code, 2 is move code
static uint8_t negative_block = 0; //determines if a block is positive (0) or negative (1)

static uint8_t New_line = 0;

static int32_t export_buffer[7]; //export buffer
static uint64_t nozzle_export[2] = {}; //nozzle export
static uint8_t parseFileName = 0;
static char fileName[30] = {0};
static uint8_t parseFileIndex = 0;


//--------------------PROTOTYPES--------------------//
static void checkCodeIdentifiers(uint8_t character);
static void parseCharacter(uint8_t character, uint8_t charecterLimit);
static void parseDecimalCharacter(uint8_t character);
static void write_to_export_buffer();
static void write_to_temp_buffer();
static void clear_nozzle_export();
static void clear_block_data();


//--------------------GETTER FUNCTIONS--------------------//
/*! \brief Returns if the command in the export buffer is
 *         new
 *         
 *         This is only valid if the caller then calls
 *         clearNewLine() if this returns true
 */
uint8_t isNewLine()
{
  return New_line;
}


/*! \brief Returns the content of the export buffer
 * 
 */
int32_t* getExportBuffer()
{
  return export_buffer;
}


/*! \brief Returns the content of the nozzle buffer
 * 
 */
uint64_t* getNozzleBuffer()
{
  return nozzle_export;
}


//--------------------SETTER FUNCTIONS--------------------//
/*! \brief Clears the new line flag in code
 * 
 *         Call this after isNewLine returns true.
 */
void clearNewLine()
{
  New_line = 0;
}


//--------------------SUPPORT FUNCTIONS--------------------//
void clear_nozzle_stated()
{
  for (int n = 0; n < 2; n++)
  {
    nozzle_stated[n] = 0; 
  }
}

static void clear_nozzle_export()
{
  for(int t = 0; t < 2; t++){
    nozzle_export[t] = 0; 
  }
}

static void clear_block_data()
{
  block_pos = 0;
  line_pos = 0;
  negative_block = 0;
  hasDecimal = -1;
  for (int n = 0; n < 7; n++)
  {
    temp_block[n] = 0;
  }
}

void clear_line_data()
{
  line_pos = 0;
  line_code = 0;
  for (int n = 0; n < 7; n++)
  {
    temp_buffer[n] = 0;
  }
}

void clear_export_buffer()
{
  for (int t = 0; t < 7; t++)
  {
    export_buffer[t] = 0;
  }
}

void clear_nozzle_history()
{
  for (int n = 0; n < 2; n++)
  {
    nozzle_history[n] = 0;
  }
}



static void checkCodeIdentifiers(uint8_t character)
{
    //Make sure that a line code has not been set if a N command
    //is send, if so, set a violation
    if(character == 'N')
    {
      if(line_code < 2){
        line_code = 1;
      }else{
        line_pos = '!';
      }    
    }
    
    //Check if the line code is not set to be a G command,
    //if so, set a violation
    else if(character == 'G')
    {
      if(!line_code){
        line_code = 2;
      }else{
        line_pos = '!';
      }
    }

    //Check if the line code is not set to be a M command,
    //if so, set a violation
    else if(character == 'M')
    {
      if(!line_code){
        line_code = 3;
      }else{
        line_pos = '!';
      }
    }

    //Check if the line code agrees with the direction and toggle commands,
    //if not, set a violation
    else if((character == 'D') & (line_code != 2))
    {
      line_pos = '!';
    }else if((character == 'T') & (line_code != 1))
    {
      line_pos = '!';
    }
}



static void parseCharacter(uint8_t character, uint8_t charecterLimit)
{
  //Check if the digit we got was a number
  if (isdigit(character))
  {
    if (block_pos < charecterLimit)
    {
      temp_block[block_pos] = character;
      block_pos ++;
    }else{
      line_pos = '!';
    }
  }

  //Set violation if the charecter isn't a newline
  else if(character != '\n')
  {
    line_pos = '!';
  }
}



static void parseDecimalCharacter(uint8_t character)
{
  //If there is a minus sign, then mark the number negative
  if (character == '-')
  {
    negative_block = 1;
  }

  else if(character == '.')
  {
    hasDecimal = block_pos;
  }
  
  //Check if the digit we got was a number
  else if (isdigit(character))
  {
    if (block_pos < 5) // For movement numerators, the amount of numbers should never exceed 5
    {
      temp_block[block_pos] = character;
      block_pos ++;
    }else{
      line_pos = '!';
    }
  }
  
  //Set violation if the charecter isn't a newline
  else if(character != '\n')
  {
    line_pos = '!';
  }
}



int32_t parseDecimalBlock()
{
  if(hasDecimal != -1)
  {
    if((block_pos - hasDecimal) > 2){
      temp_block[block_pos] = 0;
      block_pos--;
    }
    
    return atoi(temp_block) * pow(10,abs(2 - (block_pos - hasDecimal)));
  }else{
    return atoi(temp_block) * 100;
  }
  
}



static void write_to_export_buffer()
{
  clear_export_buffer();
  clear_nozzle_export();
  
  if (line_code == 1)
  {
    temp_buffer[0] = 2000; 

    for (int t = 0; t < 2; t++)
    {
      nozzle_export[t] = nozzle_history[t];
    }
  }
  
  else if (line_code < 1)
  {
    clear_nozzle_history();
  }

  export_buffer[0] = temp_buffer[0];

  for (int t = 1; t < 7; t++)
  {
    if (((temp_buffer[t] >> 30) & 3) != 0)
    {
      export_buffer[t] = temp_buffer[t];
    }
  }

  New_line = 1;

  clear_line_data();
  clear_block_data();
  clear_nozzle_stated();
}



void parseAxisIdentifier(uint8_t bufferIndex, uint8_t idNumber)
{
  if (line_pos != 9)
  {
    if(block_pos == 0){}
    
    else{
      if (negative_block == 1)
      {
        temp_buffer[bufferIndex] = -parseDecimalBlock();
      }else{
        temp_buffer[bufferIndex] = parseDecimalBlock();
      }
    }  

    temp_buffer[bufferIndex] &= ~((int32_t)3 << 30);
    temp_buffer[bufferIndex] |= (((int32_t)idNumber & 3) << 30);
  }
}


static void write_to_temp_buffer()
{
  if (line_pos != '!') //only export block if the line is still free of violations
  {
    
    switch (line_pos) //determine place in buffer
    {
    case 'G': //G identifier
      {
        temp_buffer[0] = atoi(temp_block);
        temp_buffer[0] += 1000;
      }
     break;

    case 'M': //M identifier
    {
        temp_buffer[0] = atoi(temp_block);
        if(temp_buffer[0] == 23)
        {
          temp_buffer[1] = fileName;
          temp_buffer[1] &= ~((int32_t)3 << 30);
          temp_buffer[1] |= (((int32_t)1 & 3) << 30);
          
          parseFileName = 1;
          parseFileIndex = 0;
        }
    }
    break;

    case 'N': //N identifier
      {
        int temp = atoi(temp_block);
        if (temp >= 0 && temp < NOZZLE_COUNT)
        {
          if(temp < 64)
          {
            nozzle_stated[0] |= (uint64_t)1 << temp;
          }else{
            nozzle_stated[1] |= (uint64_t)1 << (temp - 64);
          }
        }
        else
        {
          line_pos = 9;
        }
      }
      break;  

    case 'X': //X identifier
      {
        parseAxisIdentifier(1, 1);
      }
      break;

    case 'Y': //Y identifier
      {
        parseAxisIdentifier(2, 1);
      }
      break;

    case 'Z': //Z identifier
      {
        parseAxisIdentifier(3, 1);
      }
      break;

    case 'E': //E identifier
      {
        parseAxisIdentifier(4, 1);
      }
      break;

    case 'F': //F identifier
      {
        parseAxisIdentifier(5, 1);
      }
      break;

      case 'S': //S identifier
      {
        parseAxisIdentifier(6, 1);
      }
      break;

      case 'I': //I identifier
      {
        parseAxisIdentifier(3, 2);
      }
      break;

      case 'P': //P identifier
      {
        parseAxisIdentifier(5, 2);
      }
      break;

      case 'J': //J identifier
      {
        parseAxisIdentifier(6, 3);
      }
      break;
      
      case 'R': //R identifier
      {
        parseAxisIdentifier(5, 4);
      }
      break;
      
    case 'D': //Direction identifier
      {
        int temp = atoi(temp_block);
        if (temp == 0)
        {
          temp_buffer[0] -= 2; //make G1 (1000) -1, so 999. 999 is G1 with D0
        }
        else if (temp == 1)
        {
          temp_buffer[0] -= 3; //make G1 (1000) -2, so 998. 998 is G1 with D1
        }
        else
        {
          line_pos = 9;
        }
      }
      break;

      case 'T': //toggle identifier
      {
        //pack nozzle values in temp buffer
        //define if toggle on or toggle of
        int tempt = atoi(temp_block);
        if (tempt == 0 || tempt == 1)
        {
          for (int t = 0; t < NOZZLE_COUNT; t++)
          {
            
            if(t < 64){
              if((nozzle_stated[0] >> t) & 1){
                if(tempt){
                  nozzle_history[0] |= ((uint64_t)1 << t);
                }else{
                  nozzle_history[0] &= (~((uint64_t)1 << t));
                }
              }
            }else{
              if((nozzle_stated[1] >> (t - 64)) & 1){
                if(tempt){
                  nozzle_history[1] |= ((uint64_t)1 << (t - 64));
                }else{
                  nozzle_history[1] &= (~((uint64_t)1 << (t - 64)));
                }
              }
            }             
          }
        }
        else
        {
          line_pos = 9;
        }
      }
      break;


    }
  }
  clear_block_data();
}


//--------------------FUNCTIONS--------------------//
void initSDCard()
{
  clear_line_data();
  clear_export_buffer();
  clear_nozzle_history();
  clear_nozzle_stated();
}


void updateSDCard(uint8_t isSDCard)
{
  uint8_t character = 0;
  if(isSDCard)
  {
    character = getSDCharacter();
  }
  else
  {
    character = getUSBCharacter();
  }

 
  //Read a new charecter and mark the end of file if one is not available
  if ((isEndOfFile() && isSDCard) || (isEndOfCommandList() && !isSDCard))
  {
    parseFileName = 0;
    write_to_temp_buffer();
    write_to_export_buffer();
  }


  //Do not parse if we are at the end of the file
  if ((isEndOfFile() && isSDCard) || (isEndOfCommandList() && !isSDCard)){
    parseFileName = 0;
    return;
  }


  //Write to the buffer if we encounter a new line
  if (character == '\n')
  {
    write_to_temp_buffer();
    write_to_export_buffer();
    parseFileName = 0;
    return;
  }


  //If we are parsing M23 only parse following charecters as a file name
  if(parseFileName)
  {
    fileName[parseFileIndex] = character;
    parseFileIndex++;

    return;
  }


  //Store block data if we encounter a space
  if (character == ' ')
  {
    write_to_temp_buffer();
    return;
  }
  
  
  if(!line_pos){
    line_pos = character;
    checkCodeIdentifiers(character);
  }

  else if(line_pos == 'N' || line_pos == 'G' || line_pos == 'M')
  {
    //For direction numerators, the amount of numbers should never exceed 3 (0-3)
    parseCharacter(character, 3);
  }

  
  else if(line_pos == 'X' || line_pos == 'Y' || line_pos == 'Z' || line_pos == 'E' || 
          line_pos == 'F' || line_pos == 'S' || line_pos == 'I' || line_pos == 'P' ||
          line_pos == 'J' || line_pos == 'R')
  {
    parseDecimalCharacter(character);
  }


  else if(line_pos == 'D' || line_pos == 'T')
  {
    //For direction numerators, the amount of numbers should never exceed 1 (0-1)
    parseCharacter(character, 1);
  }
}
