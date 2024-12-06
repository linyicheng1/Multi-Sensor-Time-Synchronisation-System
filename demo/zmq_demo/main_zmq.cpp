#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "udp_manager.h"
#include "cam_manager.h"
#include "data_manager.h"
#include "zmq.hpp"
#include "msg_imu.pb.h"
#include "msg_image.pb.h"

void Pub(zmq::socket_t *pub, const std::string &topic, const std::string &metadata) {
  zmq::message_t topic_msg(topic.size());
  memcpy(topic_msg.data(), topic.c_str(), topic.size());
  pub->send(topic_msg, zmq::send_flags::sndmore);  // Send the topic firs

  zmq::message_t query(metadata.length());
  memcpy(query.data(), metadata.c_str(), metadata.size());
  pub->send(query, zmq::send_flags::dontwait);
}

int main() {
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  zmq::context_t ctx(10);
  zmq::socket_t zmq_publisher(ctx, ZMQ_PUB);
  zmq_publisher.bind("tcp://127.0.0.1:5555");
  /*
   本机IP地址应该在192.168.1.X网段下。
   192.168.1.168是同步板的IP地址
   The host IP address should be in the 192.168.1.X network segment.
   192.168.1.168 is the IP address of the synchronization board.
 */
  auto udp_manager = std::make_shared<UdpManager>("192.168.1.168", 8888);
  udp_manager->Start();
  CamManger::GetInstance().Initialization();
  CamManger::GetInstance().Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::vector<std::string> all_cam_names;
  DataManger::GetInstance().GetAllCamNames(all_cam_names);
  std::cout << "Number of cameras detected " << all_cam_names.size() << std::endl;
  ImuData imu_data{};
  while (zmq_publisher.connected()) {
    while (DataManger::GetInstance().GetNewImuData(imu_data)) {
      auto imu = std::make_shared<protocol::Imu>();
      imu->mutable_header()->set_stamp(imu_data.time_stamp_us * 1000);
      imu->mutable_header()->set_sensor_name("imu");
      imu->add_angular_velocity(imu_data.gx);
      imu->add_angular_velocity(imu_data.gy);
      imu->add_angular_velocity(imu_data.gz);

      imu->add_linear_acceleration(imu_data.ax);
      imu->add_linear_acceleration(imu_data.ay);
      imu->add_linear_acceleration(imu_data.az);
      auto serialized_msg = imu->SerializeAsString();
      Pub(&zmq_publisher, "imu", serialized_msg);
    }
    protocol::Image image;
    ImgData image_data{};
    for (auto &cam : all_cam_names) {
      while (DataManger::GetInstance().GetNewCamData(cam, image_data)) {
        if (image_data.image.empty()) {
          continue;
        }
        image.mutable_header()->set_stamp(image_data.time_stamp_us * 1000);
        image.mutable_header()->set_sensor_name(cam);
        image.set_name(cam);
        protocol::Mat mat;
        mat.set_rows(image_data.image.rows);
        mat.set_cols(image_data.image.cols);
        mat.set_channels(image_data.image.channels());
        mat.set_elt_type(image_data.image.type());
        mat.set_elt_size((int)image_data.image.elemSize());
        size_t data_size = image_data.image.rows * image_data.image.cols * image_data.image.elemSize();
        mat.set_data(image_data.image.data, data_size);
        image.set_name(image_data.camera_name);
        image.mutable_mat()->MergeFrom(mat);
        std::string serialized_msg = image.SerializeAsString();
        Pub(&zmq_publisher, "image", serialized_msg);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
  udp_manager->Stop();
  CamManger::GetInstance().Stop();
  zmq_publisher.close();
  return 0;
}