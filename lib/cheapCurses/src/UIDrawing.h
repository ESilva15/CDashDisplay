#ifndef __CURSES_UI_VISUALS
#define __CURSES_UI_VISUALS

#include <Arduino_GFX.h>

#define RIGHT 0
#define INVERTED 1
#define LEFT 2
#define NORMAL 3

#define DEFAULT_BORDER_THICKNESS 3
#define DEFAULT_TITLE_CONTENT_SPACING 3
#define DEFAULT_MARGIN 3

#define MAIN_BG_COLOR RGB565(20, 10, 10)
#define MAIN_FG_COLOR RGB565(255, 255, 255)

#define CHR_WIDTH(size) (6 * size)
#define CHR_HEIGHT(size) (8 * size)
#define CHR_SPACE(size) (size)

uint16_t calculateHeight(int16_t titleSize, int16_t textSize, int16_t nLines);
uint16_t calculateWidth(int16_t textSize, int16_t nChars);

#endif
