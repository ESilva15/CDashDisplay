#include "ui.h"

#include "UIDecorations.h"
#include "data.h"

DashUI::DashUI() {}

void DashUI::Update(DataPacket *p) {
  this->digiSpeedo->Update(p->speed);
  this->digiTacho->Update(p->rpm);
  this->digiGear->Update(p->gear);
  this->curLap->Update(p->currLapTime);
  this->lastLap->Update(p->lastLapTime);
  this->fuelConsumption->Update(p->FuelEst);

  for (int row = 0; row < ROWS; row++) {
    this->relative->tableData[row * COLUMNS + 0]->Update(p->standings[row].Lap);
    this->relative->tableData[row * COLUMNS + 1]->Update(
        p->standings[row].DriverName);
    this->relative->tableData[row * COLUMNS + 2]->Update(
        p->standings[row].TimeBehindString);
  }
}
