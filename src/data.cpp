#include "data.h"
#include <Arduino.h>
#include <cstring>

uint8_t packetBuffer[sizeof(DataPacket)] = {0};
int bufferIndex = 0;
bool isReceiving = false;

void SendDataRequest() {
  DataReq req = {5};
  Serial.write((uint8_t *)&req, sizeof(req));
  Serial.flush();
}

int RecvDataPacket(DataPacket *p) {
  while (Serial.available() > 0) {
    uint8_t byte = Serial.read();

    if (!isReceiving) {
      if (byte == 0x02) {
        isReceiving = true;
        bufferIndex = 0;
      }
    }
    packetBuffer[bufferIndex++] = byte;

    if (bufferIndex >= sizeof(DataPacket)) {
      Serial.flush();
      bufferIndex = 0;
      isReceiving = false;

      memcpy(p, packetBuffer, sizeof(DataPacket));

      if (p->EndMarker != 0x03) {
        // Serial2.println(F("\r////////////// DATA IS MALFORMED
        // //////////////"));
        return 1;
      }

      return 0;
      // for(int k = 0; k < sizeof(DataPacket); k++) {
      //   if (k % 8 == 0) {
      //     Serial2.printf("\r\n");
      //   }
      //   Serial2.printf(" 0x%02x", packetBuffer[k]);
      // }
      // Serial2.printf("\r\n");

      // Serial2.printf("%d / %d [%2d] || %d / %d [%2d]\n\n\r",
      // ESP.getFreeHeap(),
      //                ESP.getHeapSize(),
      //                (int)(((float)(ESP.getHeapSize() - ESP.getFreeHeap()) /
      //                       ESP.getHeapSize()) *
      //                      100),
      //                ESP.getFreePsram(), ESP.getPsramSize(),
      //                (int)(((float)(ESP.getPsramSize() - ESP.getFreePsram())
      //                /
      //                       ESP.getPsramSize()) *
      //                      100));

      // debugDataPacket(&packet);
    }
  }
  return 1;
}

void DebugDataPacket(DataPacket *p) {
  Serial2.printf("\rSize:        %zu\n", sizeof(struct DataPacket));
  Serial2.printf("\rStartMarker: 0x%02x\n", p->StartMarker);
  Serial2.printf("\rSpeed:       %s\n", p->speed);
  Serial2.printf("\rGear:        %s\n", p->gear);
  Serial2.printf("\rRPM:         %s\n", p->rpm);
  Serial2.printf("\rLapNumber:   %s\n", p->LapNumber);
  Serial2.printf("\rcurrLapTime: %s\n", p->currLapTime);
  Serial2.printf("\rlastLapTime: %s\n", p->lastLapTime);
  Serial2.printf("\rfuelEst:     %s\n", p->FuelEst);
  Serial2.printf("\rStandings:\n");
  for (int k = 0; k < 5; k++) {
    Serial2.printf("\r  Lap:         %s\n", p->standings[k].Lap);
    Serial2.printf("\r  DriverName:  %s\n", p->standings[k].DriverName);
    Serial2.printf("\r  TimeBehind:  %s\n", p->standings[k].TimeBehindString);
  }
  Serial2.printf("\rEndMarker:   0x%02x\n\n", p->EndMarker);
}
