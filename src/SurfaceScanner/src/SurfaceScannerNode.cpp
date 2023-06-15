#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_srvs/srv/trigger.hpp"
#include <cv_bridge/cv_bridge.h>
#include "Scanner.hpp"
#include <sensor_msgs/PointCloud2.h>
#include "interfaces/msg/CameraCalibrationImgs.hpp"
#include "interfaces/msg/ImagePair.hpp"
#include "interfaces/srv/CalibrateLaserImport.hpp"

using std::placeholders::_1;

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
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_calibrateScannerSrv;
    rclcpp::Service<interfaces::srv::CalibrateLaserImport>::SharedPtr m_calibrateLaserWithImportSrv;
    rclcpp::Subscription<interfaces::msg::ImagePair>::SharedPtr imgPairSub;
    rclcpp::Subscription<interfaces::msg::CameraCalibrationImgs>::SharedPtr camCalibrateImgsSub;
    rclcpp::Publisher<sensor_msgs::PointCloud2>::SharedPtr pcdPub;
    rclcpp::Publisher<sensor_msgs::PointCloud2>::SharedPtr laserPlanePub;
    void calibrateScannerSrvCallback(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response);
    void calibrateWithImportSrvCallback(const std::shared_ptr<interfaces::srv::CalibrateLaserImport::Request> request, std::shared_ptr<interfaces::srv::CalibrateLaserImport::Response> response);
    void imagePairCallback(const interfaces::msg::ImagePair & msg);
    /*
        This subscriber callback takes the list of imgs through the topic
        and edits the m_calibImgs attribute
    */
    void camCalibImgsCallback(const interfaces::msg::CameraCalibrationImgs & msg);
};

SurfaceScannerNode::SurfaceScannerNode():Node("SurfaceScannerNode"), m_Scanner(Scanner()), m_originImg(defaultMat), m_laserImg(defaultMat), m_calibImgs(std::vector<cv::Mat>())
{
    m_calibrateScannerSrv = create_service("calibrate_scanner", &calibrateScannerSrvCallback);

    m_calibrateLaserWithImportSrv = create_service("calibrate_with_import", &calibrateWithImportSrvCallback);
    
    imgPairSub = create_subscription("img_pair", 10, std::bind(&imagePairCallback, this, _1));

    camCalibrateImgsSub = create_subscription("cam_calib_imgs", 10, std::bind(&camCalibImgsCallback, this, _1));

    pcdPub = create_publisher<sensor_msgs::PointCloud2>('surface_line', 10);

    laserPlanePub = create_publisher<sensor_msgs::PointCloud2>('laser_plane', 10);

    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Surface-Scanner-Node ready!");
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

void SurfaceScannerNode::calibrateWithImportSrvCallback(const std::shared_ptr<interfaces::srv::CalibrateLaserImport::Request> request, std::shared_ptr<interfaces::srv::CalibrateLaserImport::Response> response){
    if( m_originImg.empty() || m_laserImg.empty()){
        response->success = false;
        response->message = "Scanner calibration cancelled! Missing images for laser calibration.";
    }
    else{
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request for scanner calibration with import of camera parameters!");
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Holding two images to calibrate laser.");
        RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "[origin_img: " << m_originImg.size() <<", laser_img: " << m_laserImg.size() << "]");
        m_Scanner.calibrateScannerWithImport(request->file, m_originImg, m_laserImg);

        if(!m_Scanner.isScannerCalibrated()){
            response->success = false;
            response->message = "Scanner calibration unsuccessful!";
        }
        else{
            // Publish laser lines of laser calibration
            // ToDo: complete alg later
            response->success = true;
            response->message = "Scanner calibration successful! Ready to scan.";
            resetImgs();
        }
    }
}

void SurfaceScannerNode::imagePairCallback(const interfaces::msg::ImagePair & msg){
    if(msg.is_for_laser_calib){
        try{
            //mono8 and bgr8 are the two image encodings expected by most OpenCV functions
            m_originImg = cv_bridge::toCvCopy(msg.origin_img, "bgr8")->image;
            m_laserImg = cv_bridge::toCvCopy(msg.laser_img, "bgr8")->image;
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Received two images to calibrate laser.");
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "[origin_img: " << m_originImg.size() <<", laser_img: " << m_laserImg.size() << "]");
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR(e.what());
        }
    }
    else{
        if(m_Scanner.isScannerCalibrated()){
            try{
                cv::Mat origin_img = cv_bridge::toCvCopy(msg.origin_img, "bgr8")->image;
                cv::Mat laser_img = cv_bridge::toCvCopy(msg.laser_img, "bgr8")->image;
                RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Received two images to generate surface point cloud!.");
                RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "[origin_img: " << origin_img.size() <<", laser_img: " << laser_img.size() << "]");
            }
            catch (cv_bridge::Exception& e)
            {
                ROS_ERROR(e.what());
            }
            // ToDo generate point cloud
        }
        else
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "Scanner is not calibrated!");      
    }
}

void SurfaceScannerNode::camCalibImgsCallback(const interfaces::msg::CameraCalibrationImgs & msg){
    for(std::size_t i=0, i<msg.imgs.size(), ++i){
        m_calibImgs.push_back(cv_bridge::toCvCopy(msg.imgs[i], "bgr8")->image);
    }
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Recieved list with: "<<msg.imgs.size()<<" images for intrinsic camera calibration!");
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("SurfaceScannerNode");
  rclcpp::spin(node);
  rclcpp::shutdown();
}