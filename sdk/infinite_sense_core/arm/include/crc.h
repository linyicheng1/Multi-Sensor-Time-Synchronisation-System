#pragma once
#include <cstdint>
class Crc {
 public:
  Crc();
  ~Crc();
  static uint16_t Crc16(const uint8_t *ptr, uint32_t len);
  static constexpr uint8_t start_package[3] = {0x80, 0x81, 0x82};
  static constexpr uint8_t stop_package[3] = {0x83, 0x84, 0x85};
 private:
  static uint16_t crc_tabs_[16];
};