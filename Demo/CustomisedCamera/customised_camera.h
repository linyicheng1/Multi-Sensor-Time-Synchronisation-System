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

     // 初始化相机函数
    bool Initialization();
    // 停止相机读取线程函数
    void Stop();
    // 开始相机读取线程函数
    void Start();
    // 启用相机函数
    void Enable() { is_running_ = true; }
    // 禁用相机函数
    void Disable() { is_running_ = false; }
    // 相机线程，读取相机数据
    void Receive(const std::string&) const;

private:

    CustCamManger();
    ~CustCamManger();
    // 相机线程向量，一个相机一个线程读取
    std::vector<std::thread> cam_threads_;
    // 是否正在运行标志位
    bool is_running_{false};
};
