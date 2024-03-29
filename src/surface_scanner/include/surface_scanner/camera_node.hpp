#pragma once
#include <opencv2/opencv.hpp>
#include <sensor_msgs/msg/image.h>
#include <cv_bridge/cv_bridge.h>
#include <rclcpp/rclcpp.hpp>
#include "interfaces/msg/camera_calibration_imgs.hpp"
#include "interfaces/msg/image_pair.hpp"
#include "interfaces/srv/calibrate_laser_import.hpp"
#include <std_srvs/srv/trigger.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

using namespace std;
using namespace interfaces;
using namespace cv;
using namespace std::placeholders;


class RPI_Camera_node : public rclcpp::Node
{
  public: RPI_Camera_node() : Node("camera_node"){

    srv_send_cam_calib_imgs = create_service<std_srvs::srv::Trigger>("send_cam_calib_imgs", std::bind(&RPI_Camera_node::send_cam_calib_imgs, this, _1, _2));
    srv_send_img_pair_calib = create_service<std_srvs::srv::Trigger>("send_img_pair_calib", std::bind(&RPI_Camera_node::send_img_pair_calib, this, _1, _2));
    srv_send_img_pair_surface = create_service<std_srvs::srv::Trigger>("send_img_pair_surface", std::bind(&RPI_Camera_node::send_img_pair_surface, this, _1, _2));
    srv_start_img_stream = create_service<std_srvs::srv::Trigger>("start_img_stream", std::bind(&RPI_Camera_node::start_img_stream, this, _1, _2));
    srv_stop_img_stream = create_service<std_srvs::srv::Trigger>("stop_img_stream", std::bind(&RPI_Camera_node::send_img_pair_calib, this, _1, _2));

    pub_img = create_publisher<sensor_msgs::msg::Image>("img_publisher", 10);          
    pub_imgPair = create_publisher<msg::ImagePair>("img_pair", 10);                    
    pub_imgsCalib = create_publisher<msg::CameraCalibrationImgs>("cam_calib_imgs", 10); 
    
    timer = create_wall_timer(std::chrono::seconds(1), std::bind(&RPI_Camera_node::timerCallback, this));
    timer->cancel(); //Timer starts at creation, so stop first
  }

  Mat get_image();
  vector<Mat> get_image_pair();
  void publish_image_pair(bool isForLaserCalib);
  void send_img_pair_calib(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response);
  void send_img_pair_surface(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response);
  void send_cam_calib_imgs(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response);
  void start_img_stream(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response);
  void stop_img_stream(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response);
  void timerCallback();

  private:
  VideoCapture cam;

  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_img;
  rclcpp::Publisher<msg::ImagePair>::SharedPtr pub_imgPair;
  rclcpp::Publisher<msg::CameraCalibrationImgs>::SharedPtr pub_imgsCalib;

  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_send_img_pair_calib;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_send_cam_calib_imgs;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_send_img_pair_surface;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_start_img_stream;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_stop_img_stream;

  rclcpp::TimerBase::SharedPtr timer;

};
