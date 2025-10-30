/*
ESP32-8048S043 based DashDisplay for simracing and stuff
by Eduardo Silva and others

TODO:
- Add a better communication mechanism that allows the desktop interface
to have more ways to run analytics
*/

#include "communication/commands.h"
#include "communication/packets.h"
#include "communication/identificationPacket.h"
#include "Arduino_GFX.h"
#include "HardwareSerial.h"
#include "UIDecorations.h"
// #include "UIDrawing.h"
#include "UIString.h"
// #include "UITable.h"
// #include "data.h"
#include "displaySetup.h"
#include "esp32-hal-psram.h"
// #include "ui.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <cstdint>
#include <cstring>
#include <unistd.h>
// #include <nvs.h>
// #include <nvs_flash.h>

#define ESLABS_DEVICE_ID 0x01
#define ESLABS_DEVICE_NAME "ESLabs CDashDisplay"

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
UIDecorations *gearTextDecor = new UIDecorations();
UIString gearText(gfx, UIDimensions(0, 0, 0, 0), gearTextDecor, (char *)"Gear");

void setup() {
  psramInit();
  Serial.begin(115200);

  // Initialize the debug serial object
  Serial2.begin(115200, SERIAL_8N1, UART1_RX, UART1_TX);
  Serial2.flush();

  Serial2.print(F("\r=== ESP32 SimRacing DashDisplay ===\r\n"));

  Serial2.print(F("* Initiating display\r\n"));
  initialDisplaySetup(gfx);

  // Setup the gear text box
  gearTextDecor->textSize = 7;
  gearText.dims.height =
      calculateHeight(gearTextDecor->titleSize, gearTextDecor->textSize, 1);
  gearText.dims.width = calculateWidth(gearTextDecor->textSize, 2);
  gearText.drawBox();
  gearText.Update("Disconnected");

  delay(500);
  Serial2.println("* Ready for loop");
}

uint64_t lastDataRead = 0;

bool gotAck = false;

uint64_t lastSent = 0;
void loop(void) {
  uint64_t cur = millis();
  // if (!gotAck && (cur - lastSent) >= 1000) {
  //   // Send the packet again
  //   sendHello();
  //   lastSent = cur;
  // }

  if (Serial.available() >= 1) {
    uint8_t cmd = Serial.read();
    if (cmd == CmdRequestID) {
      Serial2.println("Got identification request!");
      gearText.Update("Connecting");

      IdentificationPacket papers;
      initIdentificationPacket(&papers, ESLABS_DEVICE_NAME, ESLABS_DEVICE_ID);
      sendIdentificationPacket(&papers);
    } else if (cmd == CmdAckID) {
      Serial2.println("Got ack!");
      gearText.Update("Connected");
      gotAck = true;;
    }
  }


  // // Request data here
  // SendDataRequest();
  //
  // // Receive data
  // DataPacket telemetryPacket;
  // int res = RecvDataPacket(&telemetryPacket);
  //
  // if (res == 0) {
  //   ui->Update(&telemetryPacket);
  //
  //   lastDataRead = millis();
  // }
  //
  // // If no data is received for 5 seconds, reset the display
  // if (millis() - lastDataRead >= 5000) {
  //   Serial2.print("Resetting the display");
  // }
}
