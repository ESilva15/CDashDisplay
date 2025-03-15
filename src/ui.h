#ifndef __DASH_DISPLAY_UI
#define __DASH_DISPLAY_UI

#include "Arduino_GFX.h"
#include "UIString.h"

struct DashUI {
  UIString *rpmText;
  UIString *speedText;
  UIString *gearText;
  UIString *bestLap;
  UIString *lastLap;
  UIString *curLap;
  UIString *fuelTank;
  UIString *fuelConsumption;

  DashUI();
  void dashUISetup(Arduino_GFX *gfx);
};

#endif
