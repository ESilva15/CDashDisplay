#ifndef __WALKIE_TALKIE__
#define __WALKIE_TALKIE__

#include "communication/commands.h"
#include <cstdint>
#include <Arduino.h>

namespace WalkieTalkie {
  typedef enum {
    WaitingSTX = 0,
    RecvCMD = 1,
    RecvLen = 2,
    RecvPayload = 3,
    RecvCRC = 4,
    RecvETX = 5,
  } RecvState;

  static uint8_t tempLenBuffer[2];
  inline uint8_t buffer[4096];
  inline int16_t bufferIndex = 0;
  inline RecvState state = WaitingSTX;
  inline int16_t len = 0;
  inline uint8_t crc = 0;

  inline static void resetBuffer() {
    bufferIndex = 0;
  }

  struct ACKPacket {
    uint8_t StartMarker;
    uint8_t ACK;
    uint8_t EndMarker;
  };

  template<typename PacketType>
  void SendData(PacketType *packet) {
    Serial.write((uint8_t*)packet, sizeof(*packet));
    Serial.flush();
  }

  int16_t RecvStream(Command *command, uint8_t *payload, uint16_t payloadMax);
};

#endif
