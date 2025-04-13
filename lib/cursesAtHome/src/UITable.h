#ifndef __UI_TABLE
#define __UI_TABLE

#include "Arduino_GFX.h"
#include "UIComponent.h"
#include "UIString.h"

#define COLUMNS 3
#define ROWS 5
#define CELL_MARGIN 2

/* Very shitty table element, only works for the relative as of now
 */
struct UITable : UIElement {
  // will hold rows * columns UIStrings to fill the table
  int nRows = 0;
  int nColumns = 0;
  int *colWidths;
  UIString **tableData;

  UITable(Arduino_GFX *gfx, UIDimensions dims, UIDecorations *decor, int nR,
          int nC, int *cWidth, char *t);

  // void Update(StandingLine standings[5]);
  void setup();

  // Getters
  uint16_t getContentAreaHeight();
};

#endif
