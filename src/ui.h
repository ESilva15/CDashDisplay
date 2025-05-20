#ifndef __DASH_DISPLAY_UI
#define __DASH_DISPLAY_UI

#include "Arduino_GFX.h"
#include "UIString.h"
#include "UITable.h"
#include "UIBar.h"
#include "data.h"

struct DashUI {
  UIBar *barTacho;
  UIString *digiTacho;
  UIString *digiSpeedo;
  UIString *digiGear;
  UIString *lapDelta;
  UIString *bestLap;
  UIString *lastLap;
  UIString *curLap;
  UIString *fuelTank;
  UIString *fuelConsumption;
  UIString *brakeBias;
  UITable *relative;

  DashUI();
  void Update(DataPacket *p);
};

#endif
