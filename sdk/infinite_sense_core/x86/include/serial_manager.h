#pragma once

#include <mutex>
#include <thread>


#include "serial.h"
#include "crc.h"

class SerialManager {
  static constexpr size_t IMU_DATA_NUM = 56;
  static constexpr size_t BUF_LEN = 65540;

 public:
  explicit SerialManager(const std::string &port);

  ~SerialManager();

  [[nodiscard]] bool is_available() const { return serial.isOpen(); }
  void Start();
  void Stop();

 private:

  void Receive();

  void TimeStampSynchronization();
  Crc crc;
  std::string port;
  serial::Serial serial;
  std::thread rx_thread_;
  uint64_t T1{0};
  uint64_t T2{0};
  bool updated_t1_t2_{false};

  uint64_t frame_count_{0};

  bool started_{false};
};
