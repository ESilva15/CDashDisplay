#ifndef __UI_STRING
#define __UI_STRING

#include "UIComponent.h"

// Representation of single line strings
struct UIString : UIElement {
  static const size_t bufferSize = 64;
  char value[bufferSize];

  UIString();
  UIString(Arduino_GFX *d, UIDimensions dims, UIDecorations *decor,
           char *title);
  void Update(const char *value);
};

#endif
