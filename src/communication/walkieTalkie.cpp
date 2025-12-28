#include "walkieTalkie.h"
#include "HardwareSerial.h"
#include "logger.h"
#include "communication/commands.h"
#include "communication/communication.h"
#include <cstdint>
#include <stdint.h>
#include <Arduino.h>

namespace WalkieTalkie {
  static inline void WaitingSTXRoutine(byte *b) {
    LOG_TRACE(F("First byte is: %d\n"), *b);
    if (*b == STX) {
      state = RecvCMD;
      resetBuffer();
    }  
  }

  static inline void RecvCMDRoutine(Command *command, byte *b) {
    LOG_TRACE(F("Reading the command byte\n"));

    *command = Command(*b);
    LOG_TRACE(F("  Command: %d\n"), *b);

    state = RecvLen;

    resetBuffer();
  }

  static inline void RecvLenRoutine(byte *b) {
    LOG_TRACE(F("Reading the len\n"));
    buffer[bufferIndex++] = *b;
    // we are currently using a int16_t for the len so: 2 bytes
    if (bufferIndex > 1) {
      len = buffer[0] | (buffer[1] << 8);
      LOG_TRACE(F("Len: %d\n"), len);
      state = RecvPayload;
      resetBuffer();
    }
  }

  static inline void RecvPayloadRoutine(uint8_t *payload, byte *b) {
    LOG_TRACE(F("Reading payload: %d %s %d\n"), bufferIndex, " - ", *b);

    buffer[bufferIndex++] = *b;
    if (bufferIndex == len) {
      // put the buffer data somewhere
      memcpy(payload, buffer, len);
      state = RecvCRC;
      resetBuffer();
    }
  }

  static inline int RecvCRCRoutine(byte *b) {
    crc = *b;
    LOG_TRACE(F("Reacing CRC: %d\n"), crc);

    state = RecvETX;
    resetBuffer();

    return 0;
  }

  static inline size_t RecvETXRoutine(uint8_t *payload, byte *b) {
    LOG_TRACE(F("READING ETX\n"));
    state = WaitingSTX;

    uint8_t computedCRC = CRC8(payload, len);
    if (*b != ETX) {
      LOG_TRACE(F("Failure, last byte was: %d\n"), *b);
      return -1;
    } else if (crc != computedCRC) {
      LOG_TRACE(F("CRC don't match. Expected: %d, got: %d\n"), crc, computedCRC);
      return -2;
    } else {
      return len;
    }
  }

  int16_t RecvStream(Command *command, uint8_t *payload, uint16_t payloadMax) {
    int16_t result = 0;

    while (Serial.available() > 0) {
      uint8_t byte = Serial.read();
      switch(state) {
        case WaitingSTX: {
          WaitingSTXRoutine(&byte);
          break;
        }

        case RecvCMD: {
          RecvCMDRoutine(command, &byte);
          break;
        }

        case RecvLen: {
          RecvLenRoutine(&byte);
          break;
        }

        case RecvPayload: {
          RecvPayloadRoutine(payload, &byte);
          break;
        }

        case RecvCRC: {
          RecvCRCRoutine(&byte);
          break;
        }

        case RecvETX: {
          return RecvETXRoutine(payload, &byte);
          break;
        }
      }
    }

    return 0;
  }
};
