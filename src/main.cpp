#include "Arduino_GFX.h"
#include "HardwareSerial.h"
#include "UIDecorations.h"
#include "UIDrawing.h"
#include "UIString.h"
#include "UITable.h"
#include "data.h"
#include "debug/debug.h"
#include "displaySetup.h"
#include "esp32-hal-psram.h"
#include "ui.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <cstdint>
#include <cstring>

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
UIDecorations *rpmTextDecor = new UIDecorations();
UIDecorations *speedTextDecor = new UIDecorations();
UIDecorations *gearTextDecor = new UIDecorations();
UIDecorations *bestLapDecor = new UIDecorations();
UIDecorations *lastLapDecor = new UIDecorations();
UIDecorations *curLapDecor = new UIDecorations();
UIDecorations *fuelTankDecor = new UIDecorations();
UIDecorations *fuelConsumptionDecor = new UIDecorations();
UIDecorations *relativeDecor = new UIDecorations();

UIString rpmText(gfx, UIDimensions(0, 0, 0, 0), rpmTextDecor, (char *)"RPM");
UIString speedText(gfx, UIDimensions(0, 0, 0, 0), speedTextDecor,
                   (char *)"Speed");
UIString gearText(gfx, UIDimensions(0, 0, 0, 0), rpmTextDecor, (char *)"Gear");

UIString bestLap(gfx, UIDimensions(0, 0, 0, 0), bestLapDecor,
                 (char *)"Best Lap");
UIString lastLap(gfx, UIDimensions(0, 0, 0, 0), lastLapDecor,
                 (char *)"Last Lap");
UIString curLap(gfx, UIDimensions(0, 0, 0, 0), curLapDecor, (char *)"Cur. Lap");
UIString fuelTank(gfx, UIDimensions(0, 0, 0, 0), fuelTankDecor,
                  (char *)"Fuel Tank");
UIString fuelConsumption(gfx, UIDimensions(0, 0, 0, 0), fuelConsumptionDecor,
                         (char *)"Fuel Consumption");
UITable relative(gfx, UIDimensions(250, 250, 0, 0), relativeDecor,
                 (char *)"Relative");

void setup() {
  psramInit();
  Serial.begin(115200);

  // Initialize the debug serial object
  Serial2.begin(115200, SERIAL_8N1, UART1_RX, UART1_TX);

  Serial2.println(F("=== ESP32 SimRacing DashDisplay ==="));

  Serial2.println(F("* Initiating display"));
  initialDisplaySetup(gfx);

  // Setup the relative
  relativeDecor->textSize = 2;
  relative.setup();
  relative.drawBox();

  // Setup the rpmText box
  rpmTextDecor->textSize = 7;
  rpmText.dims.height =
      calculateHeight(rpmTextDecor->titleSize, rpmTextDecor->textSize, 1);
  rpmText.dims.width = calculateWidth(rpmTextDecor->textSize, 5);
  rpmText.horizontalCenter();
  ui->rpmText = &rpmText;
  ui->rpmText->drawBox();
  ui->rpmText->Update("12345");

  // Setup the speedText box
  speedTextDecor->textSize = 4;
  speedText.dims.height =
      calculateHeight(speedTextDecor->titleSize, speedTextDecor->textSize, 1);
  speedText.dims.width = calculateWidth(speedTextDecor->textSize, 4);
  speedText.placeRight(&rpmText);
  ui->speedText = &speedText;
  ui->speedText->drawBox();
  ui->speedText->Update("253");

  // Setup the gear text box
  gearTextDecor->textSize = 7;
  gearText.dims.height =
      calculateHeight(gearTextDecor->titleSize, gearTextDecor->textSize, 1);
  gearText.dims.width = calculateWidth(gearTextDecor->textSize, 2);
  gearText.horizontalCenter();
  gearText.dims.y = rpmText.dims.height + 5;
  ui->gearText = &gearText;
  ui->gearText->drawBox();
  ui->gearText->Update("3");

  // Setup the best lap box
  bestLapDecor->textSize = 3;
  bestLap.dims.height =
      calculateHeight(bestLapDecor->titleSize, bestLapDecor->textSize, 1);
  bestLap.dims.width = calculateWidth(bestLapDecor->textSize, 8);
  ui->bestLap = &bestLap;
  ui->bestLap->drawBox();
  ui->bestLap->Update("01:24.39");

  // Setup the last lap box
  lastLapDecor->textSize = 3;
  lastLap.dims.height =
      calculateHeight(lastLapDecor->titleSize, lastLapDecor->textSize, 1);
  lastLap.dims.width = calculateWidth(lastLapDecor->textSize, 8);
  lastLap.placeBelow(&bestLap);
  ui->lastLap = &lastLap;
  ui->lastLap->drawBox();
  ui->lastLap->Update("01:24.87");

  // Setup the cur lap box
  curLapDecor->textSize = 3;
  curLap.dims.height =
      calculateHeight(curLapDecor->titleSize, curLapDecor->textSize, 1);
  curLap.dims.width = calculateWidth(curLapDecor->textSize, 8);
  curLap.placeBelow(&lastLap);
  ui->curLap = &curLap;
  ui->curLap->drawBox();
  ui->curLap->Update("00:54.21");

  // Fuel tank
  fuelTankDecor->textSize = 3;
  fuelTank.dims.height =
      calculateHeight(fuelTankDecor->titleSize, fuelTankDecor->textSize, 1);
  fuelTank.dims.width = calculateWidth(fuelTankDecor->textSize, 12);
  fuelTank.placeBelow(&curLap);
  ui->fuelTank = &fuelTank;
  ui->fuelTank->drawBox();
  ui->fuelTank->Update("123 / 320");

  // Fuel consumption
  fuelConsumptionDecor->textSize = 3;
  fuelConsumption.dims.height = calculateHeight(
      fuelConsumptionDecor->titleSize, fuelConsumptionDecor->textSize, 1);
  fuelConsumption.dims.width =
      calculateWidth(fuelConsumptionDecor->textSize, 12);
  fuelConsumption.placeBelow(&fuelTank);
  ui->fuelConsumption = &fuelConsumption;
  ui->fuelConsumption->drawBox();
  ui->fuelConsumption->Update("10.4 | 11.2");
}

uint64_t lastDataRead = 0;
uint8_t packetBuffer[sizeof(DataPacket)];
int bufferIndex = 0;

void loop(void) {
  // Read bytes one by one
  while (Serial.available() > 0) {
    packetBuffer[bufferIndex] = Serial.read();
    bufferIndex++;

    // Check if we have a complete packet
    if (bufferIndex >= sizeof(DataPacket)) {
      // Copy the buffer into the struct
      DataPacket packet;
      memcpy(&packet, packetBuffer, sizeof(DataPacket));

      // Reset the buffer index
      bufferIndex = 0;

      // Process the packet
      // if (millis() - lastDataPrint > 50) {
      //   debugDataPacket(&packet);
      //   lastDataPrint = millis();
      // }

      // ui.speedometer->Update(packet.speed);
      //
      // ui.gear->Update(packet.gear);
      // ui.numberTach->Update(packet.rpm);
      // ui.barTach->Update(packet.rpm);

      // ui.relative->Update(packet.standings);
      for (int k = 0; k < 15; k += 3) {
        // ui.relative->tableData[k + 0].Update(packet.standings[k / 3].Lap);
        // ui.relative->tableData[k + 1].Update(
        //     packet.standings[k / 3].DriverName);
        // ui.relative->tableData[k + 2].Update(
        //     packet.standings[k / 3].TimeBehindString);
      }

      lastDataRead = millis();
    }
  }

  // If no data is received for 5 seconds, reset the display
  if (millis() - lastDataRead >= 5000) {
    // ui.speedometer->Update(0);
    // ui.numberTach->Update(0);
    // ui.gear->Update(0);
    // ui.barTach->Update(0);
  }
}
