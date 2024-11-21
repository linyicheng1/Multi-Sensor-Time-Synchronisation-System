#pragma once
#include <vector>
#include <thread>

#include "MvCameraControl.h"

class CamManger {
 public:
  static CamManger& GetInstance() {
    static CamManger instance;
    return instance;
  }
  CamManger(const CamManger&) = delete;
  CamManger& operator=(const CamManger&) = delete;

  bool Initialization();
  void Stop();
  void Start();
  void Enable() { is_running_ = true; }
  void Disable() { is_running_ = false; }
  void Restart();
 private:

  CamManger();
  ~CamManger();
  void Receive(void* handle, const std::string&) const;
  static bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);

  std::vector<int> rets_;
  std::vector<void*> handles_;
  std::vector<std::thread> cam_threads_;
  bool is_running_{false};
};
