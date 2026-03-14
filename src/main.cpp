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
#include "UIDrawing.h"
#include "UIScreen.h"
#include "communication/commands.h"
#include "communication/packets.h"
#include "communication/walkieTalkie.h"
#include "logger.h"
#include "values.h"
#include "windowPool.h"
#include "methods/methods.h"
// #include "UIDrawing.h"
#include "UIString.h"
// #include "UITable.h"
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
  Logger::Initialize(UART1_RX, UART1_TX);

  LOG_INFO(F("=== ESP32 SimRacing DashDisplay ===\r\n"));
  LOG_INFO(F("* Initiating display\r\n"));

  initialDisplaySetup(gfx);

  // Only setup the main screen after initializing the Serial2
  mainScreen.Setup("Main Window");
  UIElement *mainWindow = mainScreen.mainWindowHandle;
  mainWindow->drawBox();

  delay(500);
  LOG_INFO(F("* Ready for loop"));
}

const uint16_t PayloadMax = 2056;
uint8_t payload[PayloadMax] = {0};
char lineBuffer[64]; // Enough for "XX XX XX XX XX XX XX XX "
                     //
void print_memory_stats() {
    multi_heap_info_t info;
    
    // MALLOC_CAP_8BIT ensures we are looking at memory capable of 
    // storing data (Internal RAM + PSRAM if available)
    heap_caps_get_info(&info, MALLOC_CAP_8BIT);

    size_t total_free = info.total_free_bytes;
    size_t total_allocated = info.total_allocated_bytes;
    size_t total_size = total_free + total_allocated;
    size_t min_free = info.minimum_free_bytes; // "Low water mark"

    LOG_INFO(F("Memory Stats:\r\n"));
    LOG_INFO(F("  Total:     %u bytes\r\n"), total_size);
    LOG_INFO(F("  Used:      %u bytes\r\n"), total_allocated);
    LOG_INFO(F("  Free:      %u bytes\r\n"), total_free);
    LOG_INFO(F("  Min Free:  %u bytes (Historic peak usage)\r\n"), min_free);
}

void loop(void) {
  uint64_t cur = millis();

  Command cmd = CmdUnknown;

  if (Serial.available() > 0) {
    int16_t resp = WalkieTalkie::RecvStream(&cmd, payload, PayloadMax);
    if (resp < 0) {
      LOG_WARN(F("Failed to receive data from serial"));
      return;
    } else if (resp == 0) {
      LOG_INFO(F("No data to receive\r\n"));
      return;
    }

    LOG_INFO(F("Command: %s\n"), CommandToStr(cmd));
    LOG_INFO(F("Response Payload Len: %d\n"), resp);

    print_memory_stats();

    switch(cmd) {
      case CmdRequestID:
        IdentificationPacket papers;
        initIdentificationPacket(&papers, ESLABS_DEVICE_NAME, ESLABS_DEVICE_ID); 
        WalkieTalkie::SendData(&papers);
        break;
      case CmdCreateWindow: {
        int8_t newWindowID = Window::Create(payload, &mainScreen);
        if (newWindowID < 0) {
          LOG_WARN(F("Failed to add new window!\n"));
          break;
        }

        LOG_INFO(F("Successfully added a new window: %d\n"), newWindowID);

        // Now we return the ID of this new window - esdi should expect it
        WindowIDReply wID;
        initWindowIDReply(&wID, newWindowID);
        WalkieTalkie::SendData(&wID);

        break;
      }
      case CmdUpdateWinDims: {
        LOG_INFO(F("Updating Win DIMS\n"));
        bool res = Window::UpdateDims(payload, &mainScreen);
        if (!res) {
          LOG_WARN(F("Failed to update window dims\n"));
        }
        break;
      };
      case CmdUpdateWin: {
        LOG_INFO(F("UPDATEING WINDOW\n")); 
        bool res = Window::UpdateWindow(payload, &mainScreen);
        if (!res) {
          LOG_WARN(F("Failed to update window\n"));
        }
        break; 
      }      
      case CmdDestroyWindow: {
        uint8_t destroyedWinID = Window::Destroy(payload, &mainScreen);
        if (destroyedWinID < 0) {
          LOG_WARN(F("Failed to destroy window\n"));
          break;
        }

        LOG_INFO(F("Succesfully destroy window: %d\n"), destroyedWinID);

        break;
      }
      case CMDData: {
        LOG_WARN(F("Data Received: %d bytes\r\n"), resp);
        
        int pos = 0;
        
        for (int k = 0; k < resp; k++) {
            // Write 2 hex digits and a space to our local buffer
            // %02X ensures leading zeros (e.g., 0A instead of A)
            pos += sprintf(lineBuffer + pos, "%02X ", payload[k]);
        
            // Every 8 bytes OR if it's the very last byte in the payload
            if ((k + 1) % 8 == 0 || k == resp - 1) {
                // Send the completed line to the logger
                // We use LOG_DEBUG or similar so we don't spam [WARN] on every line
                LOG_DEBUG(F("%s\r\n"), lineBuffer);
                
                // Reset buffer position for the next line
                pos = 0;
                memset(lineBuffer, 0, sizeof(lineBuffer));
            }
        }
        LOG_DEBUG(F("\r\n"));

        Data::Parse(payload, resp, &mainScreen);

        break;
      }
      default:
        LOG_WARN(F("Unknown Command: `%d` has not been implemented yet."), cmd);
    }
  }
}
