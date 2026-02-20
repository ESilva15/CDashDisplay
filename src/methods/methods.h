#ifndef __METHODS__
#define __METHODS__

#include <cstdint>
#include <stdint.h>
#include "UIScreen.h"
#include "UIDimensions.h"
#include "UIDecorations.h"

namespace Window {
  const uint8_t ShowIDFalse = 0;
  const uint8_t ShowIDTrue = 1;

  struct UIWindowOpts {
    uint8_t ShowID;
    uint8_t WinType;
    // NOTE: I recon we need to change this yo
    char PreviewValue[32];
  };

  // Window creation
  struct UICreateWindowPacket {
    UIDimensions dims;
    UIDecorations decor;
    UIWindowOpts opts;
    char title[32]; // Note: we optimize this type of data transfer if necessary
                    // we are sending 32 - len(title) extra bytes everytime

    UICreateWindowPacket();
  };
  
  void printUIWindowPacket(UICreateWindowPacket *win);
  int8_t Create(uint8_t *payload, Curses::Screen *mainScreen);

  // Window destruction
  struct UIDestroyWindowPacket {
    int16_t WinID;
  };

  void printUIDestroyWindowPacket(UIDestroyWindowPacket *win);
  int8_t Destroy(uint8_t *payload, Curses::Screen *mainScreen);

  // NOTE: if I make the window update send these fields with the current
  // values I can reduce the code for this to a single struct instead I reckon
  // Window Dimensions Updates
  struct UpdateDimsPacket {
    int16_t wID;
    UIDimensions dims;
  };

  void printUpdateDimsPacket(UpdateDimsPacket *pkt);
  bool UpdateDims(uint8_t *payload, Curses::Screen *mainScreen);

  struct UIUpdateWindowPacket {
    int16_t WinID;
    UICreateWindowPacket data;
  };
  
  bool UpdateWindow(uint8_t *payload, Curses::Screen *mainScreen);
};

namespace Screen {
  struct UICreateScreenPacket{
    uint16_t x0;
    uint16_t y0;
    uint16_t width;
    uint16_t height;
    char title[32]; // Note: we optimize this type of data transfer if necessary
                    // we are sending 32 - len(title) extra bytes everytime
  };
}

#endif
