#ifndef __PACKETS__
#define __PACKETS__

#include <stdint.h>

typedef uint8_t PacketType;
const uint8_t identificationPacket = 0;

struct __attribute__((packed)) IdentificationPacket {
  char StartMarker;
  uint8_t DeviceID;
  PacketType PktType;
  char deviceName[32];
  char EndMarker;
};

void initIdentificationPacket(IdentificationPacket* packet, const char* name, 
                              uint8_t id);

struct __attribute__((packed)) NewWindowBody {
  char StartMarker;
  uint16_t x0;
  uint16_t y0;
  uint16_t width;
  uint16_t height;
  char title[32];
  char EndMarker;
};

#endif
