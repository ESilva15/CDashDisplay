#include "communication.h"

uint8_t CRC8(const uint8_t *data, size_t len) {
  uint8_t crc = 0x00;
  while (len--) {
    crc = pgm_read_byte(&crc8_table[crc ^ *data++]);
  }

  return crc;
}
