#ifndef __UI_DECORATIONS
#define __UI_DECORATIONS

#include "UIDrawing.h"
#include <stdint.h>

struct UIDecorations {
  bool hasBorder = true;
  uint16_t bgColor = MAIN_BG_COLOR, fgColor = MAIN_FG_COLOR;
  uint16_t titleColor = MAIN_FG_COLOR, borderColor = RED;
  uint8_t titleSize = 2, textSize = 4;

  UIDecorations();
  UIDecorations(uint16_t bg, uint16_t fg, uint16_t title, uint16_t border,
                uint8_t titleSize, uint8_t textSize);
};

#endif
