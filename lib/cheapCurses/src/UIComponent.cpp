#include "UIComponent.h"
// #include "u8g2.h"
#include "UIDrawing.h"
// #include <U8g2lib.h>
#include <cstdint>

UIElement::UIElement(Arduino_GFX *d, UIDimensions dims, UIDecorations *decor,
                     char *title)
    : display(d), dims(dims), decor(decor), title(title) {}

UIDimensions::UIDimensions(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    : x(x), y(y), width(w), height(h) {}

int16_t UIElement::getContentAreaX0() {
  return this->dims.x + DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN;
}

int16_t UIElement::getContentAreaY0() {
  return this->dims.y + this->getTitleAreaHeight() +
         DEFAULT_TITLE_CONTENT_SPACING;
}

uint16_t UIElement::getContentAreaHeight() {
  return this->dims.height - (DEFAULT_MARGIN * 2) - DEFAULT_BORDER_THICKNESS -
         this->getTitleAreaHeight();
}

uint16_t UIElement::getContentAreaWidth() {
  return this->dims.width - ((DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN) * 2);
}

int16_t UIElement::getTitleAreaX0() {
  return this->dims.x + DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN;
}

int16_t UIElement::getTitleAreaY0() { return this->dims.y; }

uint16_t UIElement::getTitleAreaHeight() {
  int16_t x = 0, y = 0;
  uint16_t w = 0, h = 0;

  this->display->setTextSize(this->decor->titleSize);
  this->display->getTextBounds((const char *)this->title, 0, 0, &x, &y, &w, &h);

  return h;
}

void UIElement::drawBox() {
  // This is the border rectangle
  for (int k = 0; k < DEFAULT_BORDER_THICKNESS; k++) {
    this->display->drawRect(
        this->dims.x + k, this->dims.y + k, this->dims.width - (k * 2),
        this->dims.height - (k * 2), this->decor->borderColor);
  }

  // For debugging purpouses
  // title area
  int16_t x = 0, y = 0;
  uint16_t w = 0, h = 0;

  // U8G2 u8g2;  // No display pin setup needed since we're only using font data
  // u8g2.setFont(u8g2_font_9x18_tf);
  // int ascent = u8g2.getAscent();

  h = this->getTitleAreaHeight();
  int16_t titleTopLeftX = this->getTitleAreaX0();
  this->display->drawRect(titleTopLeftX, this->getTitleAreaY0(), w, h, BLUE);

  // content area
  x = this->getContentAreaX0();
  y = this->getContentAreaY0();
  w = this->getContentAreaWidth();
  h = this->getContentAreaHeight();

  this->display->drawRect(x, y, w, h, BLUE);

  this->display->setCursor(titleTopLeftX, this->dims.y);
  this->display->print(this->title);
}

void UIElement::replaceString(char *oldVal, char *newVal) {
  size_t newValLen = strlen(newVal);
  size_t oldValLen = strlen(oldVal);

  int16_t x0 = this->getContentAreaX0();
  int16_t y0 = this->getContentAreaY0();

  this->display->setTextSize(this->decor->textSize);

  // If the new value is shorter, delete the extra in advance
  if (oldValLen > newValLen) {
    // If the old value is longer than the new value, we have to delete the
    // extra of the old value, which is whatever starts at the new value len
    for (size_t start = newValLen; start < oldValLen; start++) {
      this->display->setCursor(x0 + start * (CHR_WIDTH(this->decor->textSize)),
                               y0);
      this->display->setTextColor(this->decor->bgColor);
      this->display->print(oldVal[start]);
    }
  }

  for (size_t k = 0; k < newValLen; k++) {
    if (k <= oldValLen) {
      if (oldVal[k] != newVal[k]) {
        this->display->setCursor(x0 + k * (CHR_WIDTH(this->decor->textSize)),
                                 y0 + 0);
        this->display->setTextColor(this->decor->bgColor);
        this->display->print(oldVal[k]);
      }
    }

    this->display->setCursor(x0 + k * (CHR_WIDTH(this->decor->textSize)),
                             y0 + 0);
    this->display->setTextColor(this->decor->fgColor);
    this->display->print(newVal[k]);
  }
}
