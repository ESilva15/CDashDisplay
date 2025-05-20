#include "UIString.h"
#include "HardwareSerial.h"
#include "UIComponent.h"
#include <Arduino_GFX.h>

UIString::UIString() : UIElement(nullptr, {0, 0, 0, 0}, nullptr, (char *)"") {
  this->type = STRING;
  memset(this->value, 0, this->bufferSize);
}

UIString::UIString(Arduino_GFX *d, UIDimensions dims, UIDecorations *decor,
                   char *title)
    : UIElement(d, dims, decor, title) {
  this->type = STRING;
  memset(this->value, 0, this->bufferSize);
}

void UIString::Update(const char *v) {
  uint64_t time = millis();
  if ((time - this->lastUpdate) <= this->refreshRate) {
    return;
  }

  // If the value is larger than our buffer - should add a way of knowing we
  // got an error here
  if (strlen(v) >= this->bufferSize) {
    return;
  }

  // If the value hasn't changed we do not need to re-render
  if (strcmp(this->value, v) == 0) {
    return;
  }

  char newVal[this->bufferSize];
  memset(newVal, 0, this->bufferSize);
  // strncpy(newVal, v, this->bufferSize - 1);
  sprintf(newVal, "%s", v);

  char oldVal[this->bufferSize];
  memset(oldVal, 0, this->bufferSize);
  // strncpy(oldVal, v, this->bufferSize - 1);
  sprintf(oldVal, "%s", this->value);

  // Figure out the x0 and y0 for the text
  replaceString(oldVal, newVal);
  strncpy(this->value, v, this->bufferSize);
}
