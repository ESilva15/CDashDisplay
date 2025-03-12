#include "UIDrawing.h"

uint16_t calculateHeight(int16_t titleSize, int16_t textSize, int16_t nLines) {
  return CHR_HEIGHT(titleSize) + CHR_HEIGHT(textSize) +
         DEFAULT_TITLE_CONTENT_SPACING +
         (2 * (DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN));
}

uint16_t calculateWidth(int16_t textSize, int16_t nChars) {
  return CHR_WIDTH(textSize) * nChars +
         (2 * (DEFAULT_MARGIN + DEFAULT_BORDER_THICKNESS));
}
