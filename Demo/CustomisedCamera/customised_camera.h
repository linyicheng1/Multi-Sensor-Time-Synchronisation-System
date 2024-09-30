#ifndef __CUSTOMISED_CAMERA_H__
#define __CUSTOMISED_CAMERA_H__

#pragma once
#include <vector>
#include <thread>

class CustCamManger {
public:
    static CustCamManger& GetInstance() {
        static CustCamManger instance;
        return instance;
    }
    CustCamManger(const CustCamManger&) = delete;
    CustCamManger& operator=(const CustCamManger&) = delete;

    // 初始化相机
    bool Initialization();
    void Stop();
    void Start();
    void Enable() { is_running_ = true; }
    void Disable() { is_running_ = false; }
    void Receive(const std::string&) const;

private:

    CustCamManger();
    ~CustCamManger();
    std::vector<std::thread> cam_threads_;
    bool is_running_{false};
};

#endif //__CUSTOMISED_CAMERA_H__
