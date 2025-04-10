#include "data.h"
#include <Arduino.h>
#include <cstdint>
#include <cstring>

void SerializeDataPacket(DataPacket *p, uint8_t *buffer) {
  size_t offset = 0;

  memcpy(&p->StartMarker, buffer + offset, 1);
  offset += 1;
  Serial2.printf("\rStartMarker: 0x%02x\n", p->StartMarker);

  memcpy(&p->speed, buffer + offset, speedLen);
  offset += speedLen;
  Serial2.printf("\rSpeed:       %s\n", p->speed);

  memcpy(&p->gear, buffer + offset, gearLen);
  offset += gearLen;
  Serial2.printf("\rGear:        %s\n", p->gear);

  memcpy(&p->rpm, buffer + offset, rpmLen);
  offset += rpmLen;
  Serial2.printf("\rRPM:         %s\n", p->rpm);

  memcpy(&p->LapNumber, buffer + offset, lapNumberLen);
  offset += lapNumberLen;
  Serial2.printf("\rLapNumber:   %s\n", p->LapNumber);

  memcpy(&p->currLapTime, buffer + offset, currLapTimeLen);
  offset += currLapTimeLen;
  Serial2.printf("\rcurrLapTime: %s\n", p->currLapTime);

  memcpy(&p->lastLapTime, buffer + offset, lastLapTimeLen);
  offset += lastLapTimeLen;
  Serial2.printf("\rlastLapTime: %s\n", p->lastLapTime);

  memcpy(&p->FuelEst, buffer + offset, FuelEstLen);
  offset += FuelEstLen;
  Serial2.printf("\rFuelEst:     %s\n", p->FuelEst);
}
