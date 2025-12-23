#include "walkieTalkie.h"
#include "HardwareSerial.h"
#include "communication/commands.h"
#include "communication/communication.h"
#include <cstdint>
#include <stdint.h>
#include <Arduino.h>

namespace WalkieTalkie {
  // ahem - maybe refactor this inferno!
  size_t RecvData(Command *command, uint8_t *payload, uint16_t payloadMax) {
    int16_t len = 0;
    uint8_t crc = 0;
    int16_t result = 0;

    while (Serial.available() > 0) {
      uint8_t byte = Serial.read();
      switch(state) {
        case WaitingSTX:
          if (byte == STX) {
            Serial2.println(F("First byte is STX"));
            state = RecvCMD;
            resetBuffer();
          } else {
            Serial2.print(F("First byte is not STX: "));
            Serial2.println(byte);
          }
          break;

        case RecvCMD:
          Serial2.println(F("Reading the command byte"));

          *command = Command(byte);
          Serial2.print("  Command: ");
          Serial2.println(byte);

          state = RecvLen;
          resetBuffer();
          break;

        case RecvLen:
          Serial2.println(F("Reading the len"));
          buffer[bufferIndex++] = byte;
          // we are currently using a int16_t for the len so: 2 bytes
          if (bufferIndex > 1) {
            len = buffer[0] | (buffer[1] << 8);
            Serial2.print("Len: ");
            Serial2.println(len);
            state = RecvPayload;
            resetBuffer();
          }
          break;

        case RecvPayload:
          Serial2.print("Reading payload: ");
          Serial2.print(bufferIndex);
          Serial2.print(" - ");
          Serial2.println(byte);
          buffer[bufferIndex++] = byte;
          if (bufferIndex == len) {
            // put the buffer data somewhere
            memcpy(payload, buffer, len);
            state = RecvCRC;
            resetBuffer();
          }
          break;

        case RecvCRC:
          crc = byte;
          Serial2.print("Reading CRC:");
          Serial2.println(crc);

          if (crc != CRC8(payload, len)) {
            Serial2.print("CRC don't match. Expected: ");
            Serial2.print(crc);
            Serial2.print(", Got: ");
            Serial2.println(CRC8(payload, len));

            return -1;
            break;
          }

          state = RecvETX;
          resetBuffer();
          break;

        case RecvETX:
          Serial2.println("Reading ETX");
          if (byte != ETX) {
            Serial2.print("Failure, last byte was: ");
            Serial2.println(byte);
            state = WaitingSTX;
            return -1;
          } else {
            state = WaitingSTX;
            return len;
          }
          break;
      }
    }

    return 0;
  }
};
