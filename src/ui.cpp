#include "ui.h"
#include "HardwareSerial.h"
#include "data.h"
#include <cstdint>

DashUI::DashUI() {}

void DashUI::Update(DataPacket *p) {
  this->barTacho->Update(p->rpm);
  this->digiTacho->Update(p->rpm);
  this->digiSpeedo->Update(p->speed);
  this->digiGear->Update(p->gear);
  this->lapDelta->Update(p->DeltaToBestLap);
  this->bestLap->Update(p->bestLapTime);
  this->curLap->Update(p->currLapTime);
  this->lastLap->Update(p->lastLapTime);
  this->fuelConsumption->Update(p->FuelEst);
  this->brakeBias->Update(p->brakeBias);

  for (int row = 0; row < ROWS; row++) {
    this->relative->tableData[row * COLUMNS + 0]->Update(p->standings[row].Lap);
    this->relative->tableData[row * COLUMNS + 1]->Update(
        p->standings[row].DriverName);
    this->relative->tableData[row * COLUMNS + 2]->Update(
        p->standings[row].TimeBehindString);
  }
}
