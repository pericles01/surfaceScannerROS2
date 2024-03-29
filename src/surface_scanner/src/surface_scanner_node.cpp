#include "surface_scanner/surface_scanner_node.hpp"
#include <pcl_conversions/pcl_conversions.h>

SurfaceScannerNode::SurfaceScannerNode():Node("SurfaceScannerNode"), m_Scanner(Scanner()), m_originImg(defaultMat), m_laserImg(defaultMat), m_calibImgs(std::vector<cv::Mat>())
{
    m_calibrateScannerSrv = create_service<std_srvs::srv::Trigger>("calibrate_scanner", std::bind(&SurfaceScannerNode::calibrateScannerSrvCallback, this, _1, _2));

    m_calibrateLaserWithImportSrv = create_service<interfaces::srv::CalibrateLaserImport>("calibrate_with_import", std::bind(&SurfaceScannerNode::calibrateWithImportSrvCallback, this, _1, _2));
    
    imgPairSub = create_subscription<interfaces::msg::ImagePair>("img_pair", 10, std::bind(&SurfaceScannerNode::imagePairCallback, this, _1));

    camCalibrateImgsSub = create_subscription<interfaces::msg::CameraCalibrationImgs>("cam_calib_imgs", 10, std::bind(&SurfaceScannerNode::camCalibImgsCallback, this, _1));

    pcdPub = create_publisher<sensor_msgs::msg::PointCloud2>("surface_line", 10);

    laserPlanePub = create_publisher<sensor_msgs::msg::PointCloud2>("laser_plane", 10);

    RCLCPP_INFO(this->get_logger(), "Surface-Scanner-Node ready!");
}

void SurfaceScannerNode::resetImgs(){
    m_calibImgs.clear();
    m_originImg = defaultMat;
    m_laserImg = defaultMat;
}

void SurfaceScannerNode::calibrateScannerSrvCallback(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, std::shared_ptr<std_srvs::srv::Trigger::Response> response){
    auto logger = this->get_logger();
    if(m_calibImgs.empty()){
        response->success = false;
        response->message = "Scanner calibration cancelled! Missing images for intrinsic camera calibration.";
    }
    else if( m_originImg.empty() || m_laserImg.empty()){
        response->success = false;
        response->message = "Scanner calibration cancelled! Missing images for laser calibration.";
    }
    else{
        RCLCPP_INFO(logger, "Incoming request for scanner calibration!");
        RCLCPP_INFO_STREAM(logger, "Holding list of "<< m_calibImgs.size() <<" images for intrinsic calibration");
        RCLCPP_INFO_STREAM(logger, "Holding two images to calibrate laser with shape 1: "<< m_originImg.size() << " ,2: " << m_laserImg.size());
        m_Scanner.calibrateScanner(m_calibImgs, m_originImg, m_laserImg);

        if(!m_Scanner.isScannerCalibrated()){
            response->success = false;
            response->message = "Scanner calibration unsuccessful!";
        }
        else{
            //generate & publish point cloud
            sensor_msgs::msg::PointCloud2 pcd_msg;
            generatePcdMsg(m_originImg, m_laserImg, pcd_msg);
            RCLCPP_INFO(logger, "Publishing point cloud to display the laser plane! -> Topic: laser_plane");
            laserPlanePub->publish(pcd_msg);
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
        auto logger = this->get_logger();
        RCLCPP_INFO(logger, "Incoming request for scanner calibration with import of camera parameters!");
        RCLCPP_INFO(logger, "Holding two images to calibrate laser.");
        RCLCPP_INFO_STREAM(logger, "[origin_img: " << m_originImg.size() <<", laser_img: " << m_laserImg.size() << "]");
        m_Scanner.calibrateScannerWithImport(request->file, m_originImg, m_laserImg);

        if(!m_Scanner.isScannerCalibrated()){
            response->success = false;
            response->message = "Scanner calibration unsuccessful!";
        }
        else{
            //generate & publish point cloud
            sensor_msgs::msg::PointCloud2 pcd_msg;
            generatePcdMsg(m_originImg, m_laserImg, pcd_msg);
            RCLCPP_INFO(logger, "Publishing point cloud to display the laser plane! -> Topic: laser_plane");
            laserPlanePub->publish(pcd_msg);
            response->success = true;
            response->message = "Scanner calibration successful! Ready to scan.";
            resetImgs();
        }
    }
}

void SurfaceScannerNode::imagePairCallback(const interfaces::msg::ImagePair & msg){
    auto logger = this->get_logger();
    cv::Mat origin_img = cv_bridge::toCvCopy(msg.origin_img, "bgr8")->image;
    cv::Mat laser_img = cv_bridge::toCvCopy(msg.laser_img, "bgr8")->image;
    RCLCPP_INFO(logger, "Received two images to generate surface point cloud!.");
    RCLCPP_INFO_STREAM(logger, "[origin_img: " << origin_img.size() <<", laser_img: " << laser_img.size() << "]");
    
    if(msg.is_for_laser_calib){
        m_originImg = origin_img;
        m_laserImg = laser_img;
    }
    else{
        if(m_Scanner.isScannerCalibrated()){
            RCLCPP_INFO(logger, "Recieved image pair to generate surface point cloud!");
            RCLCPP_INFO_STREAM(logger, "[origin_img: " << origin_img.size() <<", laser_img: " << laser_img.size() << "]");
            // Generate point cloud
            sensor_msgs::msg::PointCloud2 pcd_msg;
            generatePcdMsg(origin_img, laser_img, pcd_msg);
            pcdPub->publish(pcd_msg);
            RCLCPP_INFO(logger, "Publish generated point cloud!");
        }
        else
            RCLCPP_ERROR(logger, "Scanner is not calibrated!");      
    }
}

void SurfaceScannerNode::camCalibImgsCallback(const interfaces::msg::CameraCalibrationImgs & msg){
    for(const auto& img : msg.imgs){
        m_calibImgs.push_back(cv_bridge::toCvCopy(img, "bgr8")->image);
    }
    RCLCPP_INFO_STREAM(this->get_logger(), "Recieved list with: "<<msg.imgs.size()<<" images for intrinsic camera calibration!");
}

void SurfaceScannerNode::generatePcdMsg(const cv::Mat &surface_img, const cv::Mat &surface_img_laser, sensor_msgs::msg::PointCloud2 &pcd_msg){
    m_Scanner.generatePcd(surface_img, surface_img_laser);
    auto point_cloud = m_Scanner.get_pcd();
    pcl::PCLPointCloud2 pcl_pc2;
    pcl::toPCLPointCloud2(*point_cloud, pcl_pc2);
    pcl_conversions::fromPCL(pcl_pc2, pcd_msg);
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  std::shared_ptr<rclcpp::Node> node = std::make_shared<SurfaceScannerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}