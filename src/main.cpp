/*
ESP32-8048S043 based DashDisplay for simracing and stuff
by Eduardo Silva and others

TODO:
- Add a better communication mechanism that allows the desktop interface
to have more ways to run analytics
*/

#include "Arduino_GFX.h"
#include "HardwareSerial.h"
#include "UIDecorations.h"
#include "UIDrawing.h"
#include "UIString.h"
#include "UITable.h"
#include "data.h"
#include "displaySetup.h"
#include "esp32-hal-psram.h"
#include "ui.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <cstdint>
#include <cstring>
#include <unistd.h>
// #include <nvs.h>
// #include <nvs_flash.h>

#define UART1_TX 19
#define UART1_RX 18

Arduino_ESP32RGBPanel *panel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* DCLK */, 45 /* R0 */,
    48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */, 5 /* G0 */, 6 /* G1 */,
    7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */, 8 /* B0 */, 3 /* B1 */,
    46 /* B2 */, 9 /* B3 */, 1 /* B4 */, 0 /*hsync_polarity*/,
    8 /* hsync_front_porch*/, 4 /* hsync_pulse_width*/,
    43 /* hsync_back_porch*/, 0 /*vsync_polarity*/, 8 /*vsync_front_porch*/,
    4 /*vsync_pulse_width*/, 12 /*vsync_back_porch*/, 1 /*pclk_active_neg*/,
    16000000 /*prefer_speed*/, false /*useBigEndian*/, 0 /*de_idle_high*/,
    0 /*pclk_idle_high*/
);

//
Arduino_GFX *gfx = new Arduino_RGB_Display(TFT_HOR_RES, TFT_VER_RES, panel, 16);

// HardwareSerial debugSerial(1);

// UIelements
DashUI *ui = new DashUI();
UIDecorations *rpmBarDecor = new UIDecorations();
UIDecorations *rpmTextDecor = new UIDecorations();
UIDecorations *speedTextDecor = new UIDecorations();
UIDecorations *gearTextDecor = new UIDecorations();
UIDecorations *lapDeltaDecor = new UIDecorations();
UIDecorations *bestLapDecor = new UIDecorations();
UIDecorations *lastLapDecor = new UIDecorations();
UIDecorations *curLapDecor = new UIDecorations();
UIDecorations *fuelTankDecor = new UIDecorations();
UIDecorations *fuelConsumptionDecor = new UIDecorations();
UIDecorations *brakeBiasDecor = new UIDecorations();
UIDecorations *relativeDecor = new UIDecorations();

UIBar rpmBar(gfx, UIDimensions(0, 0, 0, 0), rpmBarDecor, (char *)"Tach");
UIString rpmText(gfx, UIDimensions(0, 0, 0, 0), rpmTextDecor, (char *)"RPM");
UIString speedText(gfx, UIDimensions(0, 0, 0, 0), speedTextDecor,
                   (char *)"Speed");
UIString gearText(gfx, UIDimensions(0, 0, 0, 0), rpmTextDecor, (char *)"Gear");

UIString lapDelta(gfx, UIDimensions(0, 0, 0, 0), lapDeltaDecor,
                  (char *)"Delta");
UIString bestLap(gfx, UIDimensions(0, 0, 0, 0), bestLapDecor,
                 (char *)"Best Lap");
UIString lastLap(gfx, UIDimensions(0, 0, 0, 0), lastLapDecor,
                 (char *)"Last Lap");
UIString curLap(gfx, UIDimensions(0, 0, 0, 0), curLapDecor, (char *)"Cur. Lap");
UIString fuelTank(gfx, UIDimensions(0, 0, 0, 0), fuelTankDecor,
                  (char *)"Fuel Tank");
UIString fuelConsumption(gfx, UIDimensions(0, 0, 0, 0), fuelConsumptionDecor,
                         (char *)"Fuel Consumption");
UIString brakeBias(gfx, UIDimensions(0, 0, 0, 0), brakeBiasDecor,
                   (char *)"Brakes");

void setup() {
  psramInit();
  Serial.begin(115200);

  // Initialize the debug serial object
  Serial2.begin(115200, SERIAL_8N1, UART1_RX, UART1_TX);
  Serial2.flush();

  Serial2.print(F("\r=== ESP32 SimRacing DashDisplay ===\r\n"));

  Serial2.print(F("* Initiating display\r\n"));
  initialDisplaySetup(gfx);

  // Setup the relative
  // Define the width of the columns in number of chars
  int *colW = (int *)malloc(sizeof(int) * 3);
  colW[0] = 3;
  colW[1] = 18;
  colW[2] = 9;
  UITable *relative =
      new UITable(gfx, UIDimensions(250, 250, 0, 0), relativeDecor, 5, 3, colW,
                  (char *)"Relative");
  relativeDecor->textSize = 2;
  relative->dims.x = gfx->width() - relative->dims.width;
  relative->dims.y = gfx->height() - relative->dims.height;
  relative->refreshRate = 750;
  relative->setup();
  relative->drawBox();
  ui->relative = relative;

  // Setup the rpmBar box
  rpmBarDecor->textSize = 3;
  rpmBar.dims.height = 50;
  rpmBar.dims.width = 450;
  rpmBar.horizontalCenter(nullptr);
  rpmBar.dims.x += 10;
  rpmBar.range = 8;
  rpmBar.refreshRate = 50;
  ui->barTacho = &rpmBar;
  ui->barTacho->Box();
  ui->barTacho->Update((char*)"");

  // Setup the rpmText box
  rpmTextDecor->textSize = 7;
  rpmText.dims.height =
      calculateHeight(rpmTextDecor->titleSize, rpmTextDecor->textSize, 1);
  rpmText.dims.width = calculateWidth(rpmTextDecor->textSize, 5);
  rpmText.placeBelow(&rpmBar);
  rpmText.horizontalCenter(&rpmBar);
  rpmText.dims.y += 40; // We gotta fix how the bar declares its height instead
                        // of doing this here
  rpmText.refreshRate = 100;
  ui->digiTacho = &rpmText;
  ui->digiTacho->drawBox();
  ui->digiTacho->Update("-----");

  // Setup the speedText box
  speedTextDecor->textSize = 4;
  speedText.dims.height =
      calculateHeight(speedTextDecor->titleSize, speedTextDecor->textSize, 1);
  speedText.dims.width = calculateWidth(speedTextDecor->textSize, 4);
  speedText.placeBelow(&rpmBar);
  speedText.placeRight(&rpmText);
  speedText.dims.y += 40; // We gotta fix how the bar declares its height
                          // instead of doing this here
  ui->digiSpeedo = &speedText;
  ui->digiSpeedo->drawBox();
  ui->digiSpeedo->Update("---");

  // Setup the gear text box
  gearTextDecor->textSize = 7;
  gearText.dims.height =
      calculateHeight(gearTextDecor->titleSize, gearTextDecor->textSize, 1);
  gearText.dims.width = calculateWidth(gearTextDecor->textSize, 2);
  gearText.dims.y = rpmText.dims.height + 5;
  gearText.placeLeft(&rpmText);
  ui->digiGear = &gearText;
  ui->digiGear->drawBox();
  ui->digiGear->Update("-");

  // Setup the lap delta box
  lapDeltaDecor->textSize = 3;
  lapDelta.dims.height =
      calculateHeight(lapDeltaDecor->titleSize, lapDeltaDecor->textSize, 1);
  lapDelta.dims.width = calculateWidth(lapDeltaDecor->textSize, 6);
  lapDelta.placeBelow(&rpmText);
  lapDelta.horizontalCenter(&rpmText);
  ui->lapDelta = &lapDelta;
  ui->lapDelta->drawBox();
  ui->lapDelta->Update("--.-");

  // Setup the best lap box
  bestLapDecor->textSize = 3;
  bestLap.dims.height =
      calculateHeight(bestLapDecor->titleSize, bestLapDecor->textSize, 1);
  bestLap.dims.width = calculateWidth(bestLapDecor->textSize, 8);
  ui->bestLap = &bestLap;
  ui->bestLap->drawBox();
  ui->bestLap->Update("--:--.--");

  // Setup the last lap box
  lastLapDecor->textSize = 3;
  lastLap.dims.height =
      calculateHeight(lastLapDecor->titleSize, lastLapDecor->textSize, 1);
  lastLap.dims.width = calculateWidth(lastLapDecor->textSize, 8);
  lastLap.placeBelow(&bestLap);
  ui->lastLap = &lastLap;
  ui->lastLap->drawBox();
  ui->lastLap->Update("--:--.--");

  // Setup the cur lap box
  curLapDecor->textSize = 3;
  curLap.dims.height =
      calculateHeight(curLapDecor->titleSize, curLapDecor->textSize, 1);
  curLap.dims.width = calculateWidth(curLapDecor->textSize, 8);
  curLap.placeBelow(&lastLap);
  ui->curLap = &curLap;
  ui->curLap->drawBox();
  ui->curLap->Update("--:--.--");

  // Fuel tank
  fuelTankDecor->textSize = 3;
  fuelTank.dims.height =
      calculateHeight(fuelTankDecor->titleSize, fuelTankDecor->textSize, 1);
  fuelTank.dims.width = calculateWidth(fuelTankDecor->textSize, 12);
  fuelTank.placeBelow(&curLap);
  ui->fuelTank = &fuelTank;
  ui->fuelTank->drawBox();
  ui->fuelTank->Update("--- / ---");

  // Fuel consumption
  fuelConsumptionDecor->textSize = 3;
  fuelConsumption.dims.height = calculateHeight(
      fuelConsumptionDecor->titleSize, fuelConsumptionDecor->textSize, 1);
  fuelConsumption.dims.width =
      calculateWidth(fuelConsumptionDecor->textSize, 12);
  fuelConsumption.placeBelow(&fuelTank);
  ui->fuelConsumption = &fuelConsumption;
  ui->fuelConsumption->drawBox();
  ui->fuelConsumption->Update("--.- | --.-");

  // Brake bias
  brakeBiasDecor->textSize = 3;
  brakeBias.dims.height =
      calculateHeight(brakeBiasDecor->titleSize, brakeBiasDecor->textSize, 1);
  brakeBias.dims.width = calculateWidth(brakeBiasDecor->textSize, 5);
  brakeBias.placeBelow(&fuelConsumption);
  ui->brakeBias = &brakeBias;
  ui->brakeBias->drawBox();
  ui->brakeBias->Update("--.-");

  // const char *words[] = {"hel", "wor", "why", "is", "thi", "hap", "to",
  // "me"};
  //
  // int wIndex = 0;
  // while (1) {
  //   for (int row = 0; row < ROWS; row++) {
  //     for (int col = 0; col < COLUMNS; col++) {
  //       ui->relative->tableData[row * COLUMNS +
  //       col]->Update(words[wIndex++]); if (wIndex >= 8) {
  //         wIndex = 0;
  //       }
  //     }
  //   }
  // }

  // nvs_handle_t handle;
  // nvs_open("storage", NVS_READWRITE, &handle);
  // nvs_set_i32(handle, "counter", 42);
  // nvs_commit(handle);
  // nvs_close(handle);

  Serial2.println("* Ready for loop");
}

uint64_t lastDataRead = 0;
void loop(void) {
  // Request data here
  SendDataRequest();

  // Receive data
  DataPacket telemetryPacket;
  int res = RecvDataPacket(&telemetryPacket);

  if (res == 0) {
    ui->Update(&telemetryPacket);

    lastDataRead = millis();
  }

  // If no data is received for 5 seconds, reset the display
  if (millis() - lastDataRead >= 5000) {
    // ui.speedometer->Update(0);
    // ui.numberTach->Update(0);
    // ui.gear->Update(0);
    // ui.barTach->Update(0);
  }
}
