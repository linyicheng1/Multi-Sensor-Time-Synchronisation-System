#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Imu.h>

#include "udp_manager.h"
#include "cam_manager.h"
#include "data_manager.h"

inline ros::Time CreateRosTimestamp(const uint64_t timestamp_micoseconds) {
    static constexpr uint32_t kNanosecondsPerSecond = 1e9;
    const auto kTimestampU64 = timestamp_micoseconds * 1000;
    const uint32_t kTimestampSec = kTimestampU64 / kNanosecondsPerSecond;
    const uint32_t kRosTimestampNsec = kTimestampU64 - (kTimestampSec * kNanosecondsPerSecond);
    return {kTimestampSec, kRosTimestampNsec};
}

void PublishIMUData(const ros::Publisher& pub, const ImuData& imudata) {
    sensor_msgs::Imu imu_msg_data;
    imu_msg_data.header.frame_id = "/base_imu_link";
    imu_msg_data.header.stamp = CreateRosTimestamp(imudata.time_stamp_us);

    imu_msg_data.angular_velocity.x = imudata.gx;
    imu_msg_data.angular_velocity.y = imudata.gy;
    imu_msg_data.angular_velocity.z = imudata.gz;

    imu_msg_data.linear_acceleration.x = imudata.ax;
    imu_msg_data.linear_acceleration.y = imudata.ay;
    imu_msg_data.linear_acceleration.z = imudata.az;

    pub.publish(imu_msg_data);
}

int main(int argc, char** argv) {
    // Wait 5 seconds for the camera to start up.
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    ros::init(argc, argv, "CIS");
    ros::NodeHandle node;

    auto udp_manager = std::make_shared<UdpManager>("192.168.192.168", 8888);
    udp_manager->Start();

    ros::Publisher imu_pub = node.advertise<sensor_msgs::Imu>("/imu", 1000);
    CamManger::GetInstance().Initialization();
    CamManger::GetInstance().Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    image_transport::ImageTransport it(node);
    std::vector<std::string> all_cam_names;
    DataManger::GetInstance().GetAllCamNames(all_cam_names);
    std::cout << "Number of cameras detected " << all_cam_names.size() << std::endl;
    std::map<std::string, image_transport::Publisher> pub_list;
    for (auto& cam : all_cam_names) {
        pub_list[cam] = it.advertise(cam, 30);
    }
    ros::Rate loop_rate(1000);
    ImuData imudata{};
    while (node.ok()) {
        if (DataManger::GetInstance().GetNewImuData(imudata)) {
            PublishIMUData(imu_pub, imudata);
        }
        ImgData img_data{};
        for (auto& cam : all_cam_names) {
            if (DataManger::GetInstance().GetNewCamData(cam, img_data)) {
                sensor_msgs::ImagePtr msg =
                        cv_bridge::CvImage(std_msgs::Header(), "mono8", img_data.image.clone()).toImageMsg();
                msg->header.stamp = CreateRosTimestamp(img_data.time_stamp_us);
                pub_list[cam].publish(msg);
            }
        }
        loop_rate.sleep();
    }
    udp_manager->Stop();
    CamManger::GetInstance().Stop();
    return 0;
}
