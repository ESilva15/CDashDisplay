#include "methods.h"
#include "Arduino.h"
#include "logger.h"
#include "UIComponent.h"
#include "UIScreen.h"
#include <cstdint>
#include <string.h>

namespace Window {
  // Window creation
  void printUIWindowPacket(UICreateWindowPacket *win) {
    LOG_INFO(F("New window:\n"));
    LOG_INFO(F("  Dimensions:\n"));
    LOG_INFO(F("    X0    : %d\n"), win->dims.x);
    LOG_INFO(F("    Y0    : %d\n"), win->dims.y);
    LOG_INFO(F("    Width : %d\n"), win->dims.width);
    LOG_INFO(F("    Height: %d\n"), win->dims.height);
    LOG_INFO(F("  Decorations:\n"));
    LOG_INFO(F("    HasBorder   : %d\n"), win->decor.hasBorder);
    LOG_INFO(F("    BGColour    : %d\n"), win->decor.bgColor);
    LOG_INFO(F("    FGColour    : %d\n"), win->decor.fgColor);
    LOG_INFO(F("    TitleColour : %d\n"), win->decor.titleColor);
    LOG_INFO(F("    BorderColour: %d\n"), win->decor.borderColor);
    LOG_INFO(F("    TitleSize   : %d\n"), win->decor.titleSize);
    LOG_INFO(F("    TextSize    : %d\n"), win->decor.textSize);
    LOG_INFO(F("  Title: %s\n"), win->title);
  }

  UICreateWindowPacket::UICreateWindowPacket() {}

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

    childComponent->SetTitle((char *)"%s [%2d]", (const char *)win.title, childID);
    childComponent->SetUIDecorations(UIDecorations());
    childComponent->SetUIDimensions(UIDimensions(win.dims.x, win.dims.y, 
                                                 win.dims.width, win.dims.height));
    childComponent->SetDisplay(mainScreen->display);

    childComponent->drawBox();

    return childID;
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
