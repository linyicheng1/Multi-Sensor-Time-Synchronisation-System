#pragma once
#include <thread>

#include "data_manager.h"
#include "crc.h"

#include "practical_socket.h"

class UdpManager {
  static constexpr size_t IMU_DATA_NUM = 56;
  static constexpr size_t BUF_LEN = 65540;

 public:
  explicit UdpManager(std::string target_ip, uint16_t port);

  ~UdpManager();
  void Start();
  void Stop();
 private:
  void Receive();
  void TimeStampSynchronization();

  std::thread rx_thread_, tx_thread_;
  std::atomic<bool> udp_connected_{false};
  unsigned short port_;
  std::string target_ip_;
  UDPSocket sock_;
  Crc crc;

  uint64_t T1{0};
  uint64_t T2{0};
  bool updated_t1_t2_{false};

  uint64_t frame_count_{0};

  bool started_{false};
};