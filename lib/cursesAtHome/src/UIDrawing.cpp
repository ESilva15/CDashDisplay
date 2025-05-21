#include "UIDrawing.h"
#include "Arduino_GFX.h"
#include "HardwareSerial.h"
#include "UIComponent.h"
#include <cstdint>

/* TODO:
 * On the placement functions, change 5 to a variable or something
 */

// #define DEBUG

uint16_t calculateHeight(int16_t titleSize, int16_t textSize, int16_t nLines) {
  return CHR_HEIGHT(titleSize) + CHR_HEIGHT(textSize) +
         DEFAULT_TITLE_CONTENT_SPACING +
         (2 * (DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN));
}

uint16_t calculateWidth(int16_t textSize, int16_t nChars) {
  return CHR_WIDTH(textSize) * nChars +
         (2 * (DEFAULT_MARGIN + DEFAULT_BORDER_THICKNESS));
}

void UIElement::horizontalCenter(UIElement *ref) {
  uint16_t middlePoint = 0;

  if (ref != nullptr) {
    Serial2.println("We are here indeed!");
    middlePoint = ref->dims.x + (ref->dims.width / 2);
  } else {
    middlePoint = this->display->width() / 2;
  }

  this->dims.x = middlePoint - (this->dims.width / 2);
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

void UIElement::placeLeft(UIElement *ref) {
  this->dims.x = ref->dims.x - this->dims.width - 5;
}

void UIElement::drawBox() {
  // This is the border rectangle
  if (this->decor->hasBorder) {
    for (int k = 0; k < DEFAULT_BORDER_THICKNESS; k++) {
      this->display->drawRect(
          this->dims.x + k, this->dims.y + k, this->dims.width - (k * 2),
          this->dims.height - (k * 2), this->decor->borderColor);
    }
  }

  // For debugging purpouses
  // title area
  int16_t x = 0, y = 0;
  uint16_t w = 0, h = 0;

  // U8G2 u8g2;  // No display pin setup needed since we're only using font data
  // u8g2.setFont(u8g2_font_9x18_tf);
  // int ascent = u8g2.getAscent();

  h = this->getTitleAreaHeight();
  uint16_t titleAreaWidth = this->getTitleAreaWidth();
  int16_t titleTopLeftX = this->getTitleAreaX0();
#ifdef DEBUG
  // Title area bounds
  this->display->drawRect(titleTopLeftX, this->getTitleAreaY0(), titleAreaWidth,
                          h, GREEN);
#endif

  // content area
  x = this->getContentAreaX0();
  y = this->getContentAreaY0();
  w = this->getContentAreaWidth();
  h = this->getContentAreaHeight();

#ifdef DEBUG
  // Content area bounds
  this->display->drawRect(x, y, w, h, BLUE);
#endif

  if (this->decor->hasBorder) {
    // Remove the border line behind the title
    this->display->fillRect(titleTopLeftX - DEFAULT_MARGIN, this->dims.y,
                            titleAreaWidth + DEFAULT_MARGIN,
                            DEFAULT_BORDER_THICKNESS, MAIN_BG_COLOR);

    // Render the title
    this->display->setCursor(titleTopLeftX, this->dims.y);
    this->display->print(this->title);
  }

#ifdef DEBUG
  // Element bounds
  this->display->drawRect(this->dims.x, this->dims.y, this->dims.width,
                          this->dims.height, PURPLE);
#endif
}
