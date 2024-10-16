#include "customised_camera.h"
#include <opencv2/imgproc.hpp>
#include "data_manager.h"

CustCamManger::CustCamManger() = default;

CustCamManger::~CustCamManger() { Stop(); }

bool CustCamManger::Initialization() {
    // 自定义相机初始化
    return true;
}

// 根据相机数量，构造对应数量的线程，读取相机
void CustCamManger::Start() {
    // 假设有两个相机
    const std::string cam_1 = "cam1";
    const std::string cam_2 = "cam2";
    cam_threads_.emplace_back(&CustCamManger::Receive, this, cam_1);
    cam_threads_.emplace_back(&CustCamManger::Receive, this, cam_2);
    Enable();
}

void CustCamManger::Stop() {
    // 1. 关闭相机线程
    Disable();
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    for (auto &cam_thread : cam_threads_) {
        while (cam_thread.joinable()) {
            cam_thread.join();
        }
    }
    cam_threads_.clear();
    cam_threads_.shrink_to_fit();
    // 2. 关闭相机, 自定义相机关闭
}

void CustCamManger::Receive(const std::string &name) const {
    ImgData img_data;
    while (is_running_) {
        // 1. 自定义相机获取图像数据
        // 2. 将图像数据存入img_data
        // 3. 自定义相机获取曝光时间
        float expose_time_us = 0;
        // 4. 将曝光时间存入img_data t = t_trigger + t_expose / 2
        img_data.time_stamp_us = DataManger::GetInstance().GetLastTiggerTime() + static_cast<uint64_t>(expose_time_us / 2.);
        // 5. 设置相机名字
        img_data.camera_name = name;
        // img_data.image 是要存储的图像
        // 6. 将img_data存入DataManger
        DataManger::GetInstance().AddCamData(name, img_data);
        std::this_thread::sleep_for(std::chrono::milliseconds{5});
    }
}

