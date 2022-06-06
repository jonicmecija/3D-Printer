#ifndef NOZZLE_H
#define NOZZLE_H


#include "Settings.h"


#ifdef XAAR128
  #define PRINT_TYPE_INKJET
  #include "Xaar128.h"
#endif

#ifdef C6602A
  #define PRINT_TYPE_INKJET
  #include "C6602A.h"
#endif

#ifdef TTL_LASER
  #define PRINT_TYPE_LASER
  #include "Laser.h"
#endif

#ifdef MILL
  #define PRINT_TYPE_MILL
  #include "Mill.h"
#endif

#ifdef FDM
  #define PRINT_TYPE_FDM
  #include "FDMExtruder.h"
#endif

#endif
