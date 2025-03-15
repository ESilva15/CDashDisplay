#include "UIDrawing.h"
#include "UIComponent.h"
#include <cstdint>

/* TODO:
 * On the placement functions, change 5 to a variable or something
 */

uint16_t calculateHeight(int16_t titleSize, int16_t textSize, int16_t nLines) {
  return CHR_HEIGHT(titleSize) + CHR_HEIGHT(textSize) +
         DEFAULT_TITLE_CONTENT_SPACING +
         (2 * (DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN));
}

uint16_t calculateWidth(int16_t textSize, int16_t nChars) {
  return CHR_WIDTH(textSize) * nChars +
         (2 * (DEFAULT_MARGIN + DEFAULT_BORDER_THICKNESS));
}

void UIElement::horizontalCenter() {
  uint16_t scrW = this->display->width();
  this->dims.x = (scrW - this->dims.width) / 2;
}

// We can add a mode of alignment here, center, left, right, whatever
void UIElement::placeBelow(UIElement *ref) {
  // For now the default alignment will be left until I need some other
  this->dims.x = ref->dims.x;
  this->dims.y = ref->dims.y + ref->dims.height + 5;
}

void UIElement::placeRight(UIElement *ref) {
  this->dims.x = ref->dims.x + ref->dims.width + 5;
}
