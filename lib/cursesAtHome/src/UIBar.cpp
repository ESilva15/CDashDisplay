#include "UIBar.h"
#include "UIDrawing.h"

UIBar::UIBar(Arduino_GFX *d, UIDimensions dims, UIDecorations *decor,
             char *title)
    : UIElement(d, dims, decor, title) {}

void UIBar::Update(char* v) {
  unsigned long time = millis();
  if ((time - this->lastUpdate) <= this->refreshRate) {
    return;
  }

  // Conver the string to an integer
  uint32_t newVal = atoi(v);

  if (this->value == newVal) {
    // If the value hasn't changed we do not need to re-render
    return;
  }

  this->DrawBar(&this->value, &newVal);
  this->value = newVal;
}

void UIBar::DrawBar(uint32_t *oldValue, uint32_t *newValue) {
  // We can help identifying cur RPM visually by using different
  // colors on the bar, for example, at a given distance of the ideal shift
  // we can change the color gradually

  // If the new value is larger than the old one
  //   we just paint a new rectangle
  // If the new value is smaller than the old one
  //   we have to delete a chunk of the old rectangle
  int16_t startX = 0;
  int16_t len = 0;
  uint16_t colour = WHITE;

  if (*newValue > *oldValue) { // Increment the bar
    int32_t newV =
        (*newValue * (uint32_t)(this->dims.width - 4)) / (this->range * 1000);
    int32_t oldV =
        (this->value * (uint32_t)(this->dims.width - 4)) / (this->range * 1000);

    startX = oldV;
    len = newV - oldV;

  } else { // Decrement the bar
    int32_t newV =
        (*newValue * (uint32_t)(this->dims.width - 4)) / (this->range * 1000);
    int32_t oldV =
        (this->value * (uint32_t)(this->dims.width - 4)) / (this->range * 1000);

    startX = newV;
    len = oldV - newV;
    colour = BLACK;
  }
  this->display->fillRect(this->dims.x + 2 + startX, this->dims.y + 2, len,
                          this->dims.height - 4, colour);

  this->value = *newValue;
}

void UIBar::renderBlank() {
  this->display->setCursor(this->dims.x, this->dims.y);
  this->display->print("!RANGE");
}

void UIBar::Box() {
  // If the range was not set, we cannot draw our ruler
  if (this->range <= 0) {
    this->renderBlank();
    return;
  }

  // Bounding box for the bar
  this->display->drawRect(this->dims.x, this->dims.y, this->dims.width,
                          this->dims.height, RED);

  // Now we can draw the scale (this should be easy to modify if needed)
  // dividirs will be the range of the tach - 8 = 8000rpm

  // Calculate step size as a float for accurate positioning
  float step = (float)this->dims.width / this->range;
  for (int k = 0; k <= this->range; k++) {
    uint16_t x = round(k * step) + this->dims.x;

    // To ensure it doesn't go beyond the bounding box
    if (x >= this->dims.x + this->dims.width) {
      x = this->dims.x + this->dims.width - 1;
    }

    this->display->setTextColor(RED);
    this->display->drawFastVLine(x, this->dims.y + this->dims.height, 7, RED);
    this->display->setCursor(x, this->dims.y + this->dims.height + 7 + 2);
    this->display->setTextColor(WHITE);

    char legend[5];
    sprintf(legend, "%d", k);
    this->display->setTextSize(this->decor->textSize);
    this->display->print(legend);
  }
}
