#ifndef __METHODS__
#define __METHODS__

#include <cstdint>
#include <stdint.h>
#include "UIScreen.h"
#include "UIDimensions.h"
#include "UIDecorations.h"

namespace Window {
  // Window creation
  struct UICreateWindowPacket {
    UIDimensions dims;
    UIDecorations decor;
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

  // Window Updates
  struct UpdateDimsPacket {
    int16_t wID;
    UIDimensions dims;
  };

  void printUpdateDimsPacket(UpdateDimsPacket *pkt);
  bool UpdateDims(uint8_t *payload, Curses::Screen *mainScreen);
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
