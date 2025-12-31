#include "methods.h"
#include "Arduino.h"
#include "logger.h"
#include "UIComponent.h"
#include "UIScreen.h"
#include <string.h>

namespace CreateWindow {
  void printUIWindowPacket(UICreateWindowPacket *win) {
    LOG_INFO(F("New window:\n"));
    LOG_INFO(F("  X0    : %d\n"), win->x0);
    LOG_INFO(F("  Y0    : %d\n"), win->y0);
    LOG_INFO(F("  Width : %d\n"), win->width);
    LOG_INFO(F("  Height: %d\n"), win->height);
    LOG_INFO(F("  Title : %s\n"), win->title);
  }

  int8_t Create(uint8_t *payload, Curses::Screen *mainScreen) {
    // Load the payload into a struct
    CreateWindow::UICreateWindowPacket win;
    memcpy(&win, payload, sizeof(CreateWindow::UICreateWindowPacket));
    printUIWindowPacket(&win);

    UIElement *mainWindow = mainScreen->mainWindowHandle;
    int16_t childID = mainWindow->AddChild(STRING);
    if (childID < 0) {
      return -1;
    }

    UIElement *childComponent = mainWindow->GetChild(childID);

    childComponent->SetTitle((char *)"%s [%2d]", (const char *)win.title, childID);
    childComponent->SetUIDecorations(UIDecorations());
    childComponent->SetUIDimensions(UIDimensions(win.x0, win.y0, win.width, win.height));
    childComponent->SetDisplay(mainScreen->display);

    childComponent->drawBox();

    return childID;
  }
};

namespace DestroyWindow {
  void printUIDestroyWindowPacket(UIDestroyWindowPacket *win) {
    LOG_INFO(F("Destroy window:\n"));
    LOG_INFO(F("  WinID: %d\n"), win->WinID);
  }

  int8_t Destroy(uint8_t *payload, Curses::Screen *mainScreen) {
    DestroyWindow::UIDestroyWindowPacket win;
    memcpy(&win, payload, sizeof(DestroyWindow::UIDestroyWindowPacket));
    printUIDestroyWindowPacket(&win);

    UIElement *mainWindow = mainScreen->mainWindowHandle;
    mainWindow->RemoveChild(win.WinID);

    return win.WinID;
  }
};
