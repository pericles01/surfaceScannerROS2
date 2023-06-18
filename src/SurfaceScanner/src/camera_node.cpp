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

class RPI_Camera_node : public rclcpp::Node
{
  public: RPI_Camera_node() : Node("camera_node"){

    srv_send_cam_calib_imgs = create_service<std_srvs::srv::Trigger>("send_cam_calib_imgs", &send_cam_calib_imgs);
    srv_send_img_pair_calib = create_service<std_srvs::srv::Trigger>("send_img_pair_calib", &send_img_pair_calib);
    srv_send_img_pair_surface = create_service<std_srvs::srv::Trigger>("send_img_pair_surface", &send_img_pair_surface);
    srv_start_img_stream = create_service<std_srvs::srv::Trigger>("start_img_stream", &start_img_stream);
    srv_stop_img_stream = create_service<std_srvs::srv::Trigger>("stop_img_stream", &send_img_pair_calib);

    pub_img = create_publisher<sensor_msgs::msg::Image>("img_publisher", 10);          
    pub_imgPair = create_publisher<msg::ImagePair>("img_pair", 10);                    
    pub_imgsCalib = create_publisher<msg::CameraCalibrationImgs>("cam_calib_imgs", 10); 
    
    timer = create_wall_timer(std::chrono::seconds(2), std::bind(&RPI_Camera_node::timerCallback, this));
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


Mat RPI_Camera_node::get_image(){
  Mat img;
  cam.open(0); 
	if (!cam.isOpened()) {
    printf("Failed to connect to the camera!\n");
    return img;
  }
	cam.read(img);
	cam.release();
  return img;
}

vector<Mat> RPI_Camera_node::get_image_pair(){
  vector<Mat> imgs;
  imgs.push_back(get_image());
  
  //TODO turn on Laser, take a picture and then turn laser off
  imgs.push_back(get_image());

  return imgs;
}

void RPI_Camera_node::publish_image_pair(bool isForLaserCalib){
  vector<Mat> images = get_image_pair();
  auto origin = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", images[0]).toImageMsg();
  auto laser = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", images[1]).toImageMsg();

  auto msg = std::make_shared<msg::ImagePair>();

  msg->is_for_laser_calib = isForLaserCalib;
  msg->origin_img = *origin;
  msg->laser_img = *laser;

  pub_imgPair->publish(*msg);
}

void RPI_Camera_node::send_img_pair_calib(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response)  
{
  publish_image_pair(true);

  response->message = "successfully sending pair images for calib";
  response->success = true;
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service [send_img_pair_calib]: Publishing image pair on topic [img_pair]");
}

void RPI_Camera_node::send_img_pair_surface(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response)  
{
  publish_image_pair(false);

  response->message = "successfully sending pair images for surface";
  response->success = true;
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service [send_img_pair_surface]: Publishing image pair on topic [img_pair]");
}

void RPI_Camera_node::send_cam_calib_imgs(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response){
  
  std::string str = ament_index_cpp::get_package_share_directory("camera_node_rpi");

  Mat calibration_imgs[13];
  auto msg = std::make_shared<msg::CameraCalibrationImgs>();
  
  std::vector<sensor_msgs::msg::Image::SharedPtr> images;
  cv_bridge::CvImage cv_image;
  cv_image.encoding = "bgr8";
  
  for(int i=0 ; i<13 ; i++){
    calibration_imgs[i] = imread(str + "/cam_calib_imgs/frame-" + to_string(i) + ".png");
    cv_image.image = calibration_imgs[i];
    msg->imgs.push_back(*cv_image.toImageMsg());
  }

  pub_imgsCalib->publish(*msg);

  response->message = "successfully sendind calibration images";
  response->success = true;
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service [send_cam_calib_imgs]: Publishing image pair on topic [cam_calib_imgs]");

}

void RPI_Camera_node::start_img_stream(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                            shared_ptr<std_srvs::srv::Trigger::Response> response){
  
  timer->reset();
  if(timer->is_ready()){
    response->message = "Successfully started sending images!";
    response->success = true;
  }
  else{
    response->message = "Image stream is already activated!";
    response->success = false;
  }
}

void RPI_Camera_node::stop_img_stream(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                            shared_ptr<std_srvs::srv::Trigger::Response> response){
  timer->cancel();
  if(timer->is_canceled()){
    response->message = "Successfully stopped stream!";
    response->success = true;
  }
  else{
    response->message = "Image stream not activated!";
    response->success = false;
  }
}

void RPI_Camera_node::timerCallback(){
  Mat image = get_image();
  auto img_out = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", image).toImageMsg();
  sensor_msgs::msg::Image::SharedPtr msg;

  msg = img_out;

  pub_img->publish(*msg);
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "calling timer callback");
}

int main(int argc, char **argv){
  rclcpp::init(argc, argv);
  auto camera_node = std::make_shared<RPI_Camera_node>("RPI_Camera_Node");
  rclcpp::spin(camera_node);
  rclcpp::shutdown();
}

