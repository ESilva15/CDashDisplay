#ifndef __DASH_DISPLAY_DATA
#define __DASH_DISPLAY_DATA

#include <stdint.h>

const int DriverNameLen = 24;
const int TimeBehindStringLen = 16;
const int LapStringLen = 4;
const int FuelTankLen = 15;
const int FuelEstLen = 15;

struct StandingLine {
  char Lap[LapStringLen];
  char DriverName[DriverNameLen];
  char TimeBehindString[TimeBehindStringLen];
};

struct DataPacket {
  int32_t speed;
  int32_t gear;
  int32_t rpm;
  char LapNumber[5];
  // char lapDelta[10];
  char currLapTime[10];
  // char bestLapTime[10];
  char lastLapTime[10];
  // char FuelTank[FuelTankLen];
  char FuelEst[FuelEstLen];
  // int32_t position;
  StandingLine standings[5];
};

#endif
