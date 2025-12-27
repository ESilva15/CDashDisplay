#include "walkieTalkie.h"
#include "HardwareSerial.h"
#include "communication/commands.h"
#include "communication/communication.h"
#include <cstdint>
#include <stdint.h>
#include <Arduino.h>

namespace WalkieTalkie {
  static inline void WaitingSTXRoutine(byte *b) {
    if (*b == STX) {
      Serial2.println(F("First byte is STX"));
      state = RecvCMD;
      resetBuffer();
    } else {
      Serial2.print(F("First byte is not STX: "));
      Serial2.println(*b);
    }
  }

  static inline void RecvCMDRoutine(Command *command, byte *b) {
    Serial2.println(F("Reading the command byte"));

    *command = Command(*b);
    Serial2.print("  Command: ");
    Serial2.println(*b);

    state = RecvLen;

    resetBuffer();
  }

  static inline void RecvLenRoutine(byte *b) {
    Serial2.println(F("Reading the len"));
    buffer[bufferIndex++] = *b;
    // we are currently using a int16_t for the len so: 2 bytes
    if (bufferIndex > 1) {
      len = buffer[0] | (buffer[1] << 8);
      Serial2.print("Len: ");
      Serial2.println(len);
      state = RecvPayload;
      resetBuffer();
    }
  }

  static inline void RecvPayloadRoutine(uint8_t *payload, byte *b) {
    Serial2.print("Reading payload: ");
    Serial2.print(bufferIndex);
    Serial2.print(" - ");
    Serial2.println(*b);
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
    Serial2.print("Reading CRC:");
    Serial2.println(crc);

    state = RecvETX;
    resetBuffer();

    return 0;
  }

  static inline size_t RecvETXRoutine(uint8_t *payload, byte *b) {
    state = WaitingSTX;

    Serial2.println("Reading ETX");
    if (*b != ETX) {
      Serial2.print("Failure, last byte was: ");
      Serial2.println(*b);
      return -1;
    } else if (crc != CRC8(payload, len)) {
      Serial2.print("CRC don't match. Expected: ");
      Serial2.print(crc);
      Serial2.print(", Got: ");
      Serial2.println(CRC8(payload, len));

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
