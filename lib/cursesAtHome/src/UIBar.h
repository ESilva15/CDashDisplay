#ifndef _UI_BAR
#define _UI_BAR

#include "UIComponent.h"

struct UIBar : UIElement {
  // This might be the only when where its more efficient to have the data
  // as integers instead
  uint32_t value = 0; // current tach value
  uint8_t range = 0;  // define the tach range, ie: 8 for 8000rpm

  UIBar(Arduino_GFX *d, UIDimensions dims, UIDecorations *decor, char *title);

  void Update(char *val);
  void DrawBar(uint32_t *oldValue, uint32_t *newValue);
  void renderBlank();
  void Box();
};

#endif
