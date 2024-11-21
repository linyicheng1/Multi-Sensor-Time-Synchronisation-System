#pragma once

#include <mutex>
#include <vector>
#include <unordered_map>
#include <opencv2/core/mat.hpp>

#include "readerwriterqueue.h"

struct __attribute__ ((packed)) IMU {
  uint64_t time_stamp;
  float count;
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float tmp;
  float trigger;
};

struct ImuData {
  uint64_t time_stamp_us;
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float tmp;
};

struct __attribute__ ((packed))TimeStampData {
  uint16_t head;
  uint64_t mark_1;
  uint64_t mark_2;
};

struct ImgData {
  std::string camera_name;
  uint64_t time_stamp_us;
  cv::Mat image;
};

class DataManger {
 public:
  static DataManger &GetInstance() {
    static DataManger instance;
    return instance;
  }
  DataManger(const DataManger &) = delete;
  DataManger &operator=(const DataManger &) = delete;

  void AddCamData(const std::string &name, const ImgData &data);
  bool GetNewCamData(const std::string &name, ImgData &data);
  void AddImuData(const ImuData &data);
  bool GetNewImuData(ImuData &data);
  void GetAllCamNames(std::vector<std::string> &names) const;
  void SetLastTiggerTime(const uint64_t &time);
  uint64_t GetLastTiggerTime();

 private:
  DataManger();
  ~DataManger();
  std::unordered_map<std::string, std::shared_ptr<moodycamel::ReaderWriterQueue<ImgData>>> all_cams_data_{};
  moodycamel::ReaderWriterQueue<ImuData> imu_data_{512};
  std::mutex last_trigger_lock_{};
  std::atomic<uint64_t> last_imu_trigger_time_{0};
};
