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

  void printUIWindowPacket(UICreateWindowPacket *win) {
    LOG_INFO(F("New window:\n"));
    printUIDimsPacket(&win->dims);
    printUIDecorPacket(&win->decor);
    LOG_INFO(F("  Title: %s\n"), win->title);
  }

  void printUpdateDimsPacket(UpdateDimsPacket *pkt) {
    LOG_INFO(F("NEW DIMENSIONS:\n"));
    LOG_INFO(F("  TARGET WINDOW: %d\n"), pkt->wID);
    printUIDimsPacket(&pkt->dims);
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
    win->SetTitle((char*)"%s [ %d]", pkt.data.title, pkt.WinID);

    win->drawBox();

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
