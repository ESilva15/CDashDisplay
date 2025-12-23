/*
ESP32-8048S043 based DashDisplay for simracing and stuff
by Eduardo Silva and others

TODO:
- Add a better communication mechanism that allows the desktop interface
to have more ways to run analytics
*/

#include "Arduino_GFX.h"
#include "HardwareSerial.h"
#include "UIComponent.h"
#include "UIDecorations.h"
#include "UIDimensions.h"
#include "UIScreen.h"
#include "communication/commands.h"
#include "communication/packets.h"
#include "communication/walkieTalkie.h"
#include "values.h"
#include "windowPool.h"
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
Curses::Screen mainScreen(gfx, UIDimensions(0, 0, TFT_HOR_RES, TFT_VER_RES),
                          UIDecorations());

// UIelements
// UIDecorations *gearTextDecor = new UIDecorations();
// UIString gearText(gfx, UIDimensions(0, 0, 0, 0), gearTextDecor, (char
// *)"Gear");
//
// UIElement mainWindow(
//     gfx,
//     UIDimensions(0, 0, TFT_HOR_RES, TFT_VER_RES),
//     gearTextDecor,
//     (char*)"MAIN WINDOW"
// );

void setup() {
  psramInit();
  Serial.begin(115200);

  // Initialize the debug serial object
  Serial2.begin(115200, SERIAL_8N1, UART1_RX, UART1_TX);
  Serial2.flush();

  Serial2.print(F("\r=== ESP32 SimRacing DashDisplay ===\r\n"));

  Serial2.print(F("* Initiating display\r\n"));
  initialDisplaySetup(gfx);

  // Only setup the main screen after initializing the Serial2
  mainScreen.Setup("Main Window");

  // Grab a handle for the main window of the screen
  UIElement *mainWindow = mainScreen.mainWindowHandle;
  mainWindow->drawBox();

  // int16_t childID = mainWindow->AddChild(STRING);
  // if (childID < 0) {
  //   Serial2.println("Failed to add new window!");
  // } else {
  //   Serial2.print("Successfully added a new window: ");
  //   Serial2.println(childID);
  //
  //   UIElement *childComponent = mainWindow->GetChild(childID);
  //
  //   childComponent->SetTitle((char *)"%s [%2d]", (const char *)"Child Window",
  //                            childID);
  //   childComponent->SetUIDecorations(UIDecorations());
  //   childComponent->SetUIDimensions(UIDimensions(20, 20, 300, 300));
  //   childComponent->SetDisplay(mainScreen.display);
  //
  //   childComponent->drawBox();
  // }
  //
  // childID = mainWindow->AddChild(STRING);
  // if (childID < 0) {
  //   Serial2.println("Failed to add new window!");
  // } else {
  //   Serial2.print("Successfully added a new window: ");
  //   Serial2.println(childID);
  //
  //   UIElement *childComponent = mainWindow->GetChild(childID);
  //
  //   childComponent->SetTitle((char *)"%s [%2d]", (const char *)"Second Child",
  //                            childID);
  //   childComponent->SetUIDecorations(UIDecorations());
  //   childComponent->SetUIDimensions(UIDimensions(330, 20, 300, 300));
  //   childComponent->SetDisplay(mainScreen.display);
  //
  //   childComponent->drawBox();
  // }
  //
  // mainWindow->RemoveChild(1);
  //
  // childID = mainWindow->AddChild(STRING);
  // if (childID < 0) {
  //   Serial2.println("Failed to add new window!");
  // } else {
  //   Serial2.print("Successfully added a new window: ");
  //   Serial2.println(childID);
  //
  //   UIElement *childComponent = mainWindow->GetChild(childID);
  //
  //   childComponent->SetTitle((char *)"%s [%2d]", (const char *)"Third Child",
  //                            childID);
  //   childComponent->SetUIDecorations(UIDecorations());
  //   childComponent->SetUIDimensions(UIDimensions(20, 20, 300, 300));
  //   childComponent->SetDisplay(mainScreen.display);
  //
  //   childComponent->drawBox();
  // }

  WindowPool::PrintInUse();

  delay(500);
  Serial2.println("* Ready for loop");
}

uint64_t lastDataRead = 0;

bool gotAck = false;

struct header {
  uint8_t StartMarker;
  Command Cmd;
  uint8_t EndMarker;
};

void PrintHeader(header *h) {
  Serial2.println("HEADER:");
  Serial2.print("  StartMarker: ");
  Serial2.println(h->StartMarker);
  Serial2.print("  Command    : ");
  Serial2.println(CommandToStr(h->Cmd));
  Serial2.print("  EndMarker  : ");
  Serial2.println(h->EndMarker);
  Serial2.println();
}

struct UIWindowPacket {
  uint8_t StartMarker;
  uint16_t x0;
  uint16_t y0;
  uint16_t width;
  uint16_t height;
  uint8_t EndMarker;
};

uint64_t lastSent = 0;
void loop(void) {
  uint64_t cur = millis();

  // with this new way of reading data, we are just gonna read a payload
  // and get a command
  Command cmd = CmdUnknown;
  uint8_t payload[256] = {0};

  if (Serial.available() > 0) {
    size_t resp = WalkieTalkie::RecvData(&cmd, payload, 256);
    if (resp < 0) {
      Serial2.println(F("Failed to receive data from serial"));
      return;
    } else if (resp == 0) {
      Serial2.println(F("No data to receive"));
      return;
    }

    Serial2.print("Command: ");
    Serial2.println(CommandToStr(cmd));
    Serial2.print("Response: ");
    Serial2.println(resp);

    switch(cmd) {
      case CmdRequestID:
        IdentificationPacket papers;
        initIdentificationPacket(&papers, ESLABS_DEVICE_NAME, ESLABS_DEVICE_ID); 
        WalkieTalkie::SendData(&papers);
        break;
      default:
        Serial2.print(F("Command `"));
        Serial2.print(CommandToStr(cmd));
        Serial2.print(F("` has not been implemented yet."));
    }
  }

  // // Try to use a state machine for this instead? Would it be better or just more
  // // verbose and difficult to read?
  // if (Serial.available() >= 1) {
  //   // Receive the header
  //   header h = {0};
  //   int res = WalkieTalkie::RecvData(&h);
  //   if (res > 0) {
  //     PrintHeader(&h);
  //   } else {
  //     Serial2.println(F("Failed to read header"));
  //     return;
  //   }
  //
  //   // Now we receive the body
  //   switch(h.Cmd) {
  //     case CmdRequestID:
  //       // We have no body to receive, we just return data
  //       break;
  //     case CmdCreateWindow:
  //       Serial2.println("Awaiting body...");
  //       // We have to receive a body, should have plenty of data
  //       //
  //       UIWindowPacket windowData;
  //       size_t bytes = WalkieTalkie::RecvData(&windowData);
  //       if (bytes <= 0) {
  //         Serial2.println("Malformed data");
  //       }
  //
  //       Serial2.print("StartMarker: ");
  //       Serial2.println(windowData.StartMarker);
  //       Serial2.print("x0: ");
  //       Serial2.println(windowData.x0);
  //       Serial2.print("y0: ");
  //       Serial2.println(windowData.y0);
  //       Serial2.print("width: ");
  //       Serial2.println(windowData.width);
  //       Serial2.print("height: ");
  //       Serial2.println(windowData.height);
  //       // Serial2.print("title: ");
  //       // Serial2.println(windowData.title);
  //       Serial2.print("EndMarker: ");
  //       Serial2.println(windowData.EndMarker);
  //
  //       Serial2.println("Received body... Continuing");
  //       break;
  //   }
  // }
}
