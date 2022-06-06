#ifndef SDCARD_H
#define SDCARD_H

#include "stdint.h"


uint64_t* getNozzleBuffer();
int32_t* getExportBuffer();
uint8_t isNewLine();
void updateSDCard(uint8_t isSDCard);
void clearNewLine();
void clear_line_data();
void clear_export_buffer();
void clear_nozzle_history();
void clear_nozzle_stated();
void initSDCard();


#endif
