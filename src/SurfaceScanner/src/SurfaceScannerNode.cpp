#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_srvs/srv/trigger.hpp"
#include <cv_bridge/cv_bridge.h>
#include "Scanner.hpp"

class SurfaceScannerNode : public rclcpp::Node
{
public:
    SurfaceScannerNode();
    //void calibrateScannerSrv();
private:
    Scanner m_Scanner;
    cv::Mat m_originImg, m_laserImg;
    std::vector<cv::Mat> m_calibImgs;
    void resetImgs();
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr calibrateScannerSrv;
    void calibrateScannerSrvCallback(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
};

SurfaceScannerNode::SurfaceScannerNode():Node("SurfaceScannerNode"), m_Scanner(Scanner()), m_originImg(defaultMat), m_laserImg(defaultMat), m_calibImgs(std::vector<cv::Mat>())
{
    calibrateScannerSrv = create_service("calibrate_scanner", &calibrateScannerSrvCallback)
}

void SurfaceScannerNode::resetImgs(){
    m_calibImgs.clear();
    m_originImg = defaultMat;
    m_laserImg = defaultMat;
}

void SurfaceScannerNode::calibrateScannerSrvCallback(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response){
    if(m_calibImgs.empty()){
        response->success = false;
        response->message = "Scanner calibration cancelled! Missing images for intrinsic camera calibration.";
    }
    else if( m_originImg.empty() || m_laserImg.empty()){
        response->success = false;
        response->message = "Scanner calibration cancelled! Missing images for laser calibration.";
    }
    else{
        m_Scanner.calibrateScanner(m_calibImgs, m_originImg, m_laserImg);
        if(!m_Scanner.isScannerCalibrated(){
            response->success = false;
            response->message = "Scanner calibration unsuccessful!";
        })
        else{
            //ToDo publish point cloud
            //RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Publishing point cloud to display the laser plane! -> Topic: laser_plane");
            response->success = true;
            response->message = "Scanner calibration successful! Ready to scan.";
            resetImgs();
        }
    }
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("SurfaceScannerNode");
  rclcpp::spin(node);
  rclcpp::shutdown();
}