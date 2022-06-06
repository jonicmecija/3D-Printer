//--------------------LIBRARY INCLUDES--------------------//
#include <Arduino.h>
#include <SPI.h>
#include "SDSource.h"
#include "SdFat.h"


//--------------------VARIABLES--------------------//
static uint8_t end_of_file = 1;
static SdFat SD;
static SdFile fileHolder;
static SdFile rootFile;


//--------------------FUNCTIONS--------------------//
/*! \brief Initializes connection to the sd card on the
 *         spi bus
 */
void initSDSource()
{
  SD.begin(53, SD_SCK_MHZ(7));
}


/*! \brief Sets the end of file flag to signal the end
 *         of a file
 */
void setEndOfFile(uint8_t endOfFile)
{
  end_of_file = endOfFile;
}


/*! \brief Returns the status of the end of file flag
 * 
 */
uint8_t isEndOfFile()
{
  return end_of_file;
}


/*! \brief Opens a file of the name that is given in
 *         fileName
 */
uint8_t openPrintFile(char* fileName)
{
  end_of_file = 0;
  fileHolder.open(fileName, FILE_READ);
  return 0;
}


/*! \brief Closes the current file that is open
 * 
 */
void closeFile()
{
  fileHolder.close();
}


/*! \brief Initialises the iterable file name list
 * 
 */
void startFileNameList()
{
  rootFile.open("/");
}


/*! \brief Terminates the iterable file name list
 * 
 */
void endFileNameList()
{
  rootFile.close();
}


/*! \brief Gives the next file name that is in the sd card
 * 
 *         startFileNameList() needs to be called before this
 *         function and endFileNameList() needs to be called
 *         once you are done with iterating through the name
 *         list     
 */
uint8_t getNextFileName(char* fileName)
{
  uint32_t sdSearchTimeout = millis() + 1000;;

  while(sdSearchTimeout > millis())
  {
    if(fileHolder.openNext(&rootFile, O_RDONLY))
    {
      if(!fileHolder.isDir())
      {
        fileHolder.getName(fileName, 30);
        
        fileHolder.close();
        return 0;
      }

      fileHolder.close();
    }else{
      return 1;
    }
  }
  return 1;
}


/*! \brief Gets the next character that is in the currently
 *         selected file
 */
uint8_t getSDCharacter()
{
  if(fileHolder.available()){
    return fileHolder.read();  
  }else{
    end_of_file = 1;
    return 0;
  }
}


/*! \brief Creates a file with the given name
 * 
 */
void createFile(char* fileName)
{
    fileHolder.open(fileName, FILE_WRITE);  
}


/*! \brief Writes a byte to the current file
 * 
 *         Only use this after calling createFile(), do
 *         not use after using openPrintFile()
 */
void writeFile(char* fileName)
{
    fileHolder.write(fileName);  
}
