#include "UITable.h"
#include "HardwareSerial.h"
#include "UIDecorations.h"
#include "UIDrawing.h"
#include <cstdint>

#define DEBUG

UITable::UITable(Arduino_GFX *gfx, UIDimensions dims, UIDecorations *decor,
                 char *title)
    : UIElement(gfx, dims, decor, title) {
  this->type = TABLE;

  // Default decoration for every cell
  UIDecorations *cellDecor = new UIDecorations();
  cellDecor->textSize = 2;
  this->decor->textSize = 2;

  for (int k = 0; k < ROWS * COLUMNS; k++) {
    this->tableData[k].decor = cellDecor;
    this->tableData[k].dims.x = 0;
    this->tableData[k].dims.y = 0;
    this->tableData[k].dims.width = 0;
    this->tableData[k].dims.height = 0;
    this->tableData[k].display = this->display;
  }

  int cellH = CHR_HEIGHT(this->decor->textSize) + CELL_MARGIN;

  // Initialize the table dimensions
  this->dims.height = ROWS * cellH + this->getTitleAreaHeight() +
                      DEFAULT_MARGIN + DEFAULT_BORDER_THICKNESS;
  this->dims.width =
      (DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN) * 2 +
      (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN) * (3 + 24 + 5);
}

uint16_t UITable::getContentAreaHeight() {
  return this->dims.height - (DEFAULT_MARGIN * 2) - DEFAULT_BORDER_THICKNESS -
         this->getTitleAreaHeight();
}

/* Very shitty hardcoded function to get this going, only works for th
 * relative
 * POS NAME DELTA
 * 3 | 24 | 5
 */
void UITable::setup() {
  // Define the cell dimensions
  int tableCellW[] = {3 * (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN),
                      24 * (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN),
                      5 * (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN)};
  int cellH = CHR_HEIGHT(this->decor->textSize) + CELL_MARGIN;

  // Initialize the cells
  uint16_t yOffset = this->getContentAreaY0();
  for (int r = 0; r < ROWS; r++) {
    int16_t xOffset = this->getContentAreaX0();
    for (int c = 0; c < COLUMNS; c++) {
      this->tableData[r * COLUMNS + c].decor->textSize = this->decor->textSize;
      this->tableData[r * COLUMNS + c].decor->hasBorder = false;
      this->tableData[r * COLUMNS + c].dims.x = xOffset + CELL_MARGIN;
      this->tableData[r * COLUMNS + c].dims.y = (cellH * r) + yOffset;
      this->tableData[r * COLUMNS + c].dims.height = cellH;
      this->tableData[r * COLUMNS + c].dims.width = tableCellW[c];

      xOffset += tableCellW[c];

#ifdef DEBUG
      this->tableData[r * COLUMNS + c].drawBox();
      this->tableData[r * COLUMNS + c].Update("123");
#endif
    }
  }
}
