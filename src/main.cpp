#include "Arduino_GFX.h"
#include "HardwareSerial.h"
#include "UIDecorations.h"
#include "UIDrawing.h"
#include "UIString.h"
#include "backlightControl.h"
#include "data.h"
#include "esp32-hal-psram.h"
#include "ui.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <cstdint>
#include <cstring>
// #include <U8g2lib.h>

#define UART1_TX 19
#define UART1_RX 18

#define SCREEN_ORIENTATION 2
#define SCREEN_LANDSCAPE // comment out for portrait

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

/* More display class:
 * https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
// bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);
/*Set to your screen resolution*/
#define TFT_HOR_RES 800
#define TFT_VER_RES 480
//
Arduino_GFX *gfx = new Arduino_RGB_Display(TFT_HOR_RES, TFT_VER_RES, panel, 16);

HardwareSerial debugSerial(1);

// UIelements
DashUI *ui = new DashUI();
UIDecorations *rpmTextDecor = new UIDecorations();

UIString rpmText(gfx, UIDimensions(37, 35, 333, 111), rpmTextDecor,
                 (char *)"RPM");

void initialDisplaySetup() {
  psramInit();
  gfx->begin();
  // gfx->setFont(u8g2_font_9x18_tf);

  // Set screen & touch to the same rotation
  debugSerial.println(F("* Setting up display"));
  gfx->setRotation(SCREEN_ORIENTATION);
  backlightSetup();
  gfx->fillScreen(MAIN_BG_COLOR);
  gfx->setTextColor(MAIN_FG_COLOR);
}

void setup() {
  // psramInit();
  uint64_t start = millis();
  debugSerial.begin(115200, SERIAL_8N1, UART1_RX, UART1_TX);
  Serial.begin(115200);

  // Serial.begin(115200);
  debugSerial.println(F("=== ESP32 SimRacing DashDisplay ==="));

  debugSerial.println(F("* Initiating display"));
  initialDisplaySetup();

  // Setup the rpmText box
  rpmTextDecor->textSize = 4;
  rpmText.dims.height =
      calculateHeight(rpmTextDecor->titleSize, rpmTextDecor->textSize, 1);
  rpmText.dims.width = calculateWidth(rpmTextDecor->textSize, 5);
  ui->rpmText = &rpmText;
  ui->rpmText->drawBox();
  ui->rpmText->Update("12345");
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

      // gfx->setCursor(20, 20);
      // gfx->println(packet.speed);
      // gfx->setCursor(20, 40);
      // gfx->println(packet.rpm);
      // gfx->setCursor(20, 60);
      // gfx->println(packet.gear);
      // gfx->setCursor(20, 80);
      // gfx->println(packet.FuelEst);
      // gfx->setCursor(20, 100);
      // gfx->println(packet.LapNumber);
      // gfx->setCursor(20, 120);
      // gfx->println(packet.currLapTime);
      // gfx->setCursor(20, 140);
      // gfx->println(packet.lastLapTime);

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
