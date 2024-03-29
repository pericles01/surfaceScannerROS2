#include "surface_scanner/camera_node.hpp"
#include <filesystem>

namespace fs = std::filesystem;

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
  RCLCPP_INFO(this->get_logger(), "Service [send_img_pair_calib]: Publishing image pair on topic [img_pair]");
}

void RPI_Camera_node::send_img_pair_surface(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response)  
{
  publish_image_pair(false);

  response->message = "successfully sending pair images for surface";
  response->success = true;
  RCLCPP_INFO(this->get_logger(), "Service [send_img_pair_surface]: Publishing image pair on topic [img_pair]");
}

void RPI_Camera_node::send_cam_calib_imgs(const shared_ptr<std_srvs::srv::Trigger::Request> request,     
                               shared_ptr<std_srvs::srv::Trigger::Response> response){
  
  std::string path = ament_index_cpp::get_package_share_directory("camera_node_rpi");
  fs::directory_entry dir(path);
  Mat calibration_imgs;
  auto msg = std::make_shared<msg::CameraCalibrationImgs>();
  cv_bridge::CvImage cv_image;
  cv_image.encoding = "bgr8";
  vector<cv::String> fn;

  if(dir.is_directory())
    glob(path+"/*.png", fn, false); //only fetch .png images in the dir
  else
    RCLCPP_ERROR(this->get_logger(), "Path for image calibration doesn't exist");

  vector<cv::Mat> data;
  cv::glob(path,fn,true); // recurse
  for (size_t k=0; k<fn.size(); ++k)
  {
      calibration_imgs = cv::imread(fn[k]);
      if (calibration_imgs.empty()) continue; //only proceed if sucsessful for later processing
      cv_image.image = calibration_imgs;
      msg->imgs.push_back(*cv_image.toImageMsg());
  }

  pub_imgsCalib->publish(*msg);

  response->message = "successfully sendind calibration images";
  response->success = true;
  RCLCPP_INFO(this->get_logger(), "Service [send_cam_calib_imgs]: Publishing image pair on topic [cam_calib_imgs]");

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
  sensor_msgs::msg::Image::SharedPtr msg = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", image).toImageMsg();

  pub_img->publish(*msg);
  RCLCPP_INFO(this->get_logger(), "calling timer callback");
}

int main(int argc, char **argv){
  rclcpp::init(argc, argv);
  auto camera_node = std::make_shared<RPI_Camera_node>();
  rclcpp::spin(camera_node);
  rclcpp::shutdown();
  return 0;
}

