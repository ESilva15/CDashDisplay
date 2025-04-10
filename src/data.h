#ifndef __DASH_DISPLAY_DATA
#define __DASH_DISPLAY_DATA

#include <stdint.h>

struct DataReq {
  uint8_t type;
};

const int speedLen = 5;
const int gearLen = 3;
const int rpmLen = 6;
const int lapNumberLen = 5;
const int currLapTimeLen = 10;
const int lastLapTimeLen = 10;
const int DriverNameLen = 24;
const int TimeBehindStringLen = 16;
const int LapStringLen = 4;
const int FuelTankLen = 15;
const int FuelEstLen = 15;

#pragma pack(push, 1)
struct StandingLine {
  char Lap[LapStringLen];
  char DriverName[DriverNameLen];
  char TimeBehindString[TimeBehindStringLen];
};

struct DataPacket {
  uint8_t StartMarker;
  char speed[speedLen];
  char gear[gearLen];
  char rpm[rpmLen];
  char LapNumber[lapNumberLen];
  char currLapTime[currLapTimeLen];
  char lastLapTime[lastLapTimeLen];
  char FuelEst[FuelEstLen];
  StandingLine standings[5];
  uint8_t EndMarker;
};
#pragma pack(pop)

void SerializeDataPacket(DataPacket *p, uint8_t *buffer);

#endif
