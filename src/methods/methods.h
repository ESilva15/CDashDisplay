#ifndef __METHODS__
#define __METHODS__

#include <cstdint>
#include <stdint.h>
#include "UIScreen.h"

namespace CreateWindow {
  struct UICreateWindowPacket {
    uint16_t x0;
    uint16_t y0;
    uint16_t width;
    uint16_t height;
    char title[32]; // Note: we optimize this type of data transfer if necessary
                    // we are sending 32 - len(title) extra bytes everytime
  };
  
  void printUIWindowPacket(UICreateWindowPacket *win);
  int8_t Create(uint8_t *payload, Curses::Screen *mainScreen);
};

namespace DestroyWindow {
};

#endif
