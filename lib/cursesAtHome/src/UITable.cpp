#include "UITable.h"
#include "HardwareSerial.h"
#include "UIDecorations.h"
#include "UIDrawing.h"
#include <cstdint>

#define DEBUG

UITable::UITable(Arduino_GFX *gfx, UIDimensions dims, UIDecorations *decor,
                 int nRows, int nCols, int *colWidths, char *title)
    : UIElement(gfx, dims, decor, title) {
  this->type = TABLE;
  this->nRows = nRows;
  this->nColumns = nCols;
  this->tableData = new UIString *[this->nRows * this->nColumns];
  this->colWidths = colWidths;

  // Default decoration for every cell
  this->decor->textSize = 2;

  for (int k = 0; k < ROWS * COLUMNS; k++) {
    UIDecorations *cellDecor = new UIDecorations();
    cellDecor->textSize = this->decor->textSize;

    UIDimensions dims = {0, 0, 0, 0};

    this->tableData[k] = new UIString(display, dims, cellDecor, (char *)"");
  }

  int cellH = CHR_HEIGHT(this->decor->textSize) + CELL_MARGIN;

  // Initialize the table dimensions
  //// Calculate the height
  this->dims.height = ROWS * cellH + this->getTitleAreaHeight() +
                      DEFAULT_MARGIN + DEFAULT_BORDER_THICKNESS;

  //// Calculate the width
  for (int c = 0; c < this->nColumns; c++) {
    this->dims.width +=
        (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN) * this->colWidths[c];
  }
  this->dims.width += (DEFAULT_BORDER_THICKNESS + DEFAULT_MARGIN) * 2;
}

uint16_t UITable::getContentAreaHeight() {
  return this->dims.height - (DEFAULT_MARGIN * 2) - DEFAULT_BORDER_THICKNESS -
         this->getTitleAreaHeight();
}

/* 
 * This was hardcoded for the relative and had a warning for that but
 * I recon its dynamic enough for now
 */
void UITable::setup() {
  int cellH = CHR_HEIGHT(this->decor->textSize) + CELL_MARGIN;

  // Initialize the cells
  uint16_t yOffset = this->getContentAreaY0();
  for (int r = 0; r < ROWS; r++) {
    int16_t xOffset = this->getContentAreaX0();
    for (int c = 0; c < COLUMNS; c++) {
      this->tableData[r * COLUMNS + c]->decor->textSize = this->decor->textSize;
      this->tableData[r * COLUMNS + c]->decor->hasBorder = false;
      this->tableData[r * COLUMNS + c]->dims.x = xOffset + CELL_MARGIN;
      this->tableData[r * COLUMNS + c]->dims.y = (cellH * r) + yOffset;
      this->tableData[r * COLUMNS + c]->dims.height = cellH;
      this->tableData[r * COLUMNS + c]->dims.width =
          this->colWidths[c] * (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN);
      memset(this->tableData[r * COLUMNS + c]->value, 0,
             this->tableData[r * COLUMNS + c]->bufferSize);

      xOffset +=
          this->colWidths[c] * (CHR_WIDTH(this->decor->textSize) + CELL_MARGIN);

#ifdef DEBUG
      this->tableData[r * COLUMNS + c]->drawBox();
      this->tableData[r * COLUMNS + c]->Update("---");
#endif
    }
  }
}
