#ifndef SDWRAPPER_H
#define SDWRAPPER_H

void initSDSource();
void setEndOfFile(uint8_t endOfFile);
uint8_t isEndOfFile();
uint8_t openPrintFile(char* fileName);
void closeFile();
uint8_t getSDCharacter();
uint8_t getNextFileName(char* fileName);
void endFileNameList();
void startFileNameList();
void createFile(char* fileName);
void writeFile(char* fileName);

#endif
