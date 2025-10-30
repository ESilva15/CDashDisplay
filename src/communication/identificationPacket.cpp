#include "identificationPacket.h"
#include <Arduino.h>
#include <cstdint>
#include <cstring>

void initIdentificationPacket(IdentificationPacket* packet, const char* name,
    uint8_t id) {
  memset(packet, 0, sizeof(*packet));

  packet->StartMarker = 0x02; // Start of text
  packet->EndMarker = 0x03;   // End of text
  packet->DeviceID = id;
  packet->PktType = identificationPacket;

  // snprintf(packet->deviceName, sizeof(packet->deviceName), "%s", name);
  strncpy(packet->deviceName, name, strlen(name));
  packet->deviceName[strlen(name)] = '\0';
}

void sendIdentificationPacket(IdentificationPacket* packet) {
  Serial.write((uint8_t*)packet, sizeof(*packet));
  Serial.flush();
}
