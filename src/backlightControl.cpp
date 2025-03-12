#include "backlightControl.h"
#include "esp32-hal-ledc.h"
#include <Arduino.h>

void setBrightness(uint32_t value, uint32_t valueMax) {
  uint32_t duty = (4095 / valueMax) * min(value, valueMax);
  ledcWrite(BRIGHTNESS_CHANNEL, duty);
}

void backlightSetup() {
  ledcSetup(BRIGHTNESS_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(TFT_BL, BRIGHTNESS_CHANNEL);
  setBrightness(DEFAULT_BRIGHTNESS);
}
