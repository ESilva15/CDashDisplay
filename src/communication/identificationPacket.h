#ifndef __IDENTIFICATION_PACKET__
#define __IDENTIFICATION_PACKET__

#include "packets.h"
#include <cstdint>
#include <stdint.h>

// eventually try this out with __attrubute__((packed))
struct __attribute__((packed)) IdentificationPacket {
  char StartMarker;
  uint8_t DeviceID;
  PacketType PktType;
  char deviceName[32];
  char EndMarker;
};

void initIdentificationPacket(IdentificationPacket* packet, const char* name, 
    uint8_t id);
void sendIdentificationPacket(IdentificationPacket* packet);

#endif
