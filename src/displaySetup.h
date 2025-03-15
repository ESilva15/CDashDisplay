#ifndef __BLCTL
#define __BLCTL

#include "Arduino_GFX.h"
#include <stdint.h>

#define TFT_BL 2
#define DEFAULT_BRIGHTNESS 255
#define BRIGHTNESS_CHANNEL 0
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 12

#define SCREEN_ORIENTATION 2
#define SCREEN_LANDSCAPE

/* More display class:
 * https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
// bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);
/*Set to your screen resolution*/
#define TFT_HOR_RES 800
#define TFT_VER_RES 480

void backlightSetup();
void setBrightness(uint32_t value, uint32_t valueMax = 255);
void initialDisplaySetup(Arduino_GFX *gfx);

#endif
