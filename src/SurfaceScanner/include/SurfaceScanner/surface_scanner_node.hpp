#pragma once
#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"
#include <cv_bridge/cv_bridge.h>
#include "SurfaceScanner/scanner.hpp"
#include <sensor_msgs/msg/point_cloud2.hpp>
//#include "interfaces/include/interfaces/interfaces/msg/camera_calibration_imgs.hpp"
//#include "interfaces/include/interfaces/interfaces/msg/image_pair.hpp"
//#include "interfaces/include/interfaces/interfaces/srv/calibrate_laser_import.hpp"

#include "interfaces/msg/image_pair.hpp"
#include "interfaces/msg/camera_calibration_imgs.hpp"
#include "interfaces/srv/calibrate_laser_import.hpp"


using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

class SurfaceScannerNode : public rclcpp::Node
{
public:
    SurfaceScannerNode();
private:
    Scanner m_Scanner;
    cv::Mat m_originImg, m_laserImg;
    std::vector<cv::Mat> m_calibImgs;
    void resetImgs();
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_calibrateScannerSrv;
    rclcpp::Service<interfaces::srv::CalibrateLaserImport>::SharedPtr m_calibrateLaserWithImportSrv;
    rclcpp::Subscription<interfaces::msg::ImagePair>::SharedPtr imgPairSub;
    rclcpp::Subscription<interfaces::msg::CameraCalibrationImgs>::SharedPtr camCalibrateImgsSub;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pcdPub;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr laserPlanePub;
    void calibrateScannerSrvCallback(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
    void calibrateWithImportSrvCallback(const std::shared_ptr<interfaces::srv::CalibrateLaserImport::Request> request, std::shared_ptr<interfaces::srv::CalibrateLaserImport::Response> response);
    void imagePairCallback(const interfaces::msg::ImagePair & msg);
    /*
        This subscriber callback takes the list of imgs through the topic
        and edits the m_calibImgs attribute
    */
    void camCalibImgsCallback(const interfaces::msg::CameraCalibrationImgs & msg);
};