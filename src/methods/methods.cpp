#include "methods.h"
#include "Arduino.h"
#include "UIDecorations.h"
#include "UIDrawing.h"
#include "communication/packets.h"
#include "logger.h"
#include "UIComponent.h"
#include "UIScreen.h"
#include <cstdint>
#include <string.h>

namespace Window {
  // Window creation
  void printUIDimsPacket(UIDimensions *dims) {
    LOG_INFO(F("  Dimensions:\n"));
    LOG_INFO(F("    X0    : %d\n"), dims->x);
    LOG_INFO(F("    Y0    : %d\n"), dims->y);
    LOG_INFO(F("    Width : %d\n"), dims->width);
    LOG_INFO(F("    Height: %d\n"), dims->height);
  }

  void printUIDecorPacket(UIDecorations *decor) {
    LOG_INFO(F("  Decorations:\n"));
    LOG_INFO(F("    HasBorder   : %d\n"), decor->hasBorder);
    LOG_INFO(F("    BGColour    : %d\n"), decor->bgColor);
    LOG_INFO(F("    FGColour    : %d\n"), decor->fgColor);
    LOG_INFO(F("    TitleColour : %d\n"), decor->titleColor);
    LOG_INFO(F("    BorderColour: %d\n"), decor->borderColor);
    LOG_INFO(F("    TitleSize   : %d\n"), decor->titleSize);
    LOG_INFO(F("    TextSize    : %d\n"), decor->textSize);
  }

  void printUIOptionsPacket(UIWindowOpts* opts) {
    LOG_INFO(F("  Options:\n"));
    LOG_INFO(F("    ShowID      : %d\n"), opts->ShowID);
    LOG_INFO(F("    Type        : %d\n"), opts->WinType);
    LOG_INFO(F("    PreviewValue: %s\n"), opts->PreviewValue);
  }

  void printUIWindowPacket(UICreateWindowPacket *win) {
    LOG_INFO(F("New window:\n"));
    printUIDimsPacket(&win->dims);
    printUIDecorPacket(&win->decor);
    printUIOptionsPacket(&win->opts);
    LOG_INFO(F("  Title: %s\n"), win->title);
  }

  void printUpdateDimsPacket(UpdateDimsPacket *pkt) {
    LOG_INFO(F("NEW DIMENSIONS:\n"));
    LOG_INFO(F("  TARGET WINDOW: %d\n"), pkt->wID);
    printUIDimsPacket(&pkt->dims);
  }

  UICreateWindowPacket::UICreateWindowPacket() {}

  void setTitleWithOpts(UICreateWindowPacket* win, UIElement* elem, int16_t ID) {
    if (win->opts.ShowID == ShowIDTrue) {
      elem->SetTitle((char *)"%s [%2d]", (const char *)win->title, ID);
    } else {
      elem->SetTitle((char *)"%s", (const char *)win->title);
    }
  }

  int8_t Create(uint8_t *payload, Curses::Screen *mainScreen) {
    // Load the payload into a struct
    // UICreateWindowPacket* win;
    UICreateWindowPacket win;
    // Maybe find a better way to copy this - if we had metadata to the payload
    // for example
    memcpy((void*)&win, payload, sizeof(UICreateWindowPacket));
    printUIWindowPacket(&win);

    UIElement *mainWindow = mainScreen->mainWindowHandle;
    int16_t childID = mainWindow->AddChild(STRING);
    if (childID < 0) {
      return -1;
    }

    UIElement *childComponent = mainWindow->GetChild(childID);

    setTitleWithOpts(&win, childComponent, childID);
    childComponent->SetUIDecorations(win.decor);
    childComponent->SetUIDimensions(UIDimensions(win.dims.x, win.dims.y, 
                                                 win.dims.width, win.dims.height));
    childComponent->SetDisplay(mainScreen->display);

    childComponent->drawBox();
    childComponent->Update(win.opts.PreviewValue, true);

    return childID;
  }

  bool UpdateDims(uint8_t *payload, Curses::Screen *mainScreen) {
    UpdateDimsPacket pkt;
    memcpy((void*)&pkt, payload, sizeof(UpdateDimsPacket));
    printUpdateDimsPacket(&pkt);

    UIElement *mainWindow = mainScreen->mainWindowHandle;
    UIElement* win = mainWindow->GetChild(pkt.wID);
    if (win == NULL) {
      return false;
    }

    // Delete the old window
    fillRect(win->dims.x, win->dims.y, win->dims.width,
             win->dims.height, win->decor.bgColor, win->display);

    // Update the windows dimensions
    win->dims = pkt.dims;

    // Redraw the window
    win->drawBox();
    win->Redraw();

    return true;
  }

  bool UpdateWindow(uint8_t *payload, Curses::Screen *mainScreen) {
    UIUpdateWindowPacket pkt;
    memcpy((void*)&pkt, payload, sizeof(UICreateWindowPacket));

    printUIWindowPacket(&pkt.data);

    UIElement* mainWindow = mainScreen->mainWindowHandle;
    UIElement* win = mainWindow->GetChild(pkt.WinID);
    if (win == NULL) {
      return false;
    }

    // Delete the old window
    fillRect(win->dims.x, win->dims.y, win->dims.width,
             win->dims.height, win->decor.bgColor, win->display);

    // Update the windows dimensons, decor and title
    win->dims = pkt.data.dims;
    win->decor = pkt.data.decor;
    setTitleWithOpts(&pkt.data, win, pkt.WinID);

    win->drawBox();
    win->Update(pkt.data.opts.PreviewValue, true);

    return true;
  }

  // Window destruction
  void printUIDestroyWindowPacket(UIDestroyWindowPacket *win) {
    LOG_INFO(F("Destroy window:\n"));
    LOG_INFO(F("  WinID: %d\n"), win->WinID);
  }

  int8_t Destroy(uint8_t *payload, Curses::Screen *mainScreen) {
    UIDestroyWindowPacket win;
    memcpy(&win, payload, sizeof(UIDestroyWindowPacket));
    printUIDestroyWindowPacket(&win);

    UIElement *mainWindow = mainScreen->mainWindowHandle;
    mainWindow->RemoveChild(win.WinID);

    return win.WinID;
  }
};

namespace Data {
  uint8_t Parse(uint8_t *payload, size_t payloadSize) {
    uint16_t curPos = 0;

    for (; curPos < payloadSize;) {
      // Get the data type from the current position
      uint8_t type = payload[curPos];
    
      switch (type) {
        case DataTypeUINT8: {
          uint8_t value = payload[curPos + 1];
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 2;
          break;
        }
        case DataTypeINT8: {
          int8_t value = (int8_t)payload[curPos + 1];
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 2;
          break;
        }
        case DataTypeUINT16: {
          uint16_t value;
          memcpy(&value, payload + curPos + 1, 2);

          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 3; // 1 (Type) + 2 (Value)
          break;
        }
        case DataTypeINT16: {
          int16_t value;
          memcpy(&value, payload + curPos + 1, 2);
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 3;
          break;
        }
        case DataTypeUINT32: {
          uint32_t value;
          memcpy(&value, payload + curPos + 1, 4);
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 5; // 1 (Type) + 4 (Value)
          break;
        }
        case DataTypeINT32: {
          int32_t value;
          memcpy(&value, payload + curPos + 1, 4);
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 5;
          break;
        }
        case DataTypeUINT64: {
          uint64_t value;
          memcpy(&value, payload + curPos + 1, 8);
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 9; // 1 (Type) + 8 (Value)
          break;
        }
        case DataTypeINT64: {
          int64_t value;
          memcpy(&value, payload + curPos + 1, 8);
          LOG_WARN(F("RECEIVED: %d\n"), value);

          curPos += 9;
          break;
        }
        case DataTypeCHAR: {
          uint8_t value = payload[curPos + 1];
          LOG_WARN(F("RECEIVED: %c\n"), value);

          curPos += 2;
          break;
        }
        case DataTypeSTRING: {
          // Strings have: Type (1), Len (1), Data (Len)
          uint8_t len = payload[curPos + 1];
          char strValue[len + 1];
          memcpy(strValue, payload + curPos + 2, len);
          strValue[len] = '\0'; // Null terminator
                                //
          LOG_WARN(F("RECEIVED [%d]: %s\n"), len, strValue);
          
          curPos += (2 + len); 
          break;
        }
        default:
          // If we hit an unknown type, we are desynced. 
          // Better to stop than to read garbage.
          LOG_ERROR(F("Unknown Type 0x%02X at pos %d"), type, curPos);
      }
    }

    return 0;
  }
}
