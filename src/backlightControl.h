#ifndef __BLCTL
#define __BLCTL

#include <stdint.h>

#define TFT_BL 2
#define DEFAULT_BRIGHTNESS 255
#define BRIGHTNESS_CHANNEL 0
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 12

void backlightSetup();
void setBrightness(uint32_t value, uint32_t valueMax = 255);

#endif
