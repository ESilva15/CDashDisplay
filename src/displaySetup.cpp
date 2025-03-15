#include "displaySetup.h"
#include "UIDrawing.h"
#include "esp32-hal-ledc.h"
#include <Arduino.h>
#include <Arduino_GFX.h>

void setBrightness(uint32_t value, uint32_t valueMax) {
  uint32_t duty = (4095 / valueMax) * min(value, valueMax);
  ledcWrite(BRIGHTNESS_CHANNEL, duty);
}

void backlightSetup() {
  ledcSetup(BRIGHTNESS_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(TFT_BL, BRIGHTNESS_CHANNEL);
  setBrightness(DEFAULT_BRIGHTNESS);
}

void initialDisplaySetup(Arduino_GFX *gfx) {
  gfx->begin();
  // gfx->setFont(u8g2_font_9x18_tf);

  // Set screen & touch to the same rotation
  gfx->setRotation(SCREEN_ORIENTATION);
  backlightSetup();
  gfx->fillScreen(MAIN_BG_COLOR);
  gfx->setTextColor(MAIN_FG_COLOR);
}
