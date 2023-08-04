#include "SurfaceScanner/surface_scanner_node.hpp"

SurfaceScannerNode::SurfaceScannerNode():Node("SurfaceScannerNode"), m_Scanner(Scanner()), m_originImg(defaultMat), m_laserImg(defaultMat), m_calibImgs(std::vector<cv::Mat>())
{
    m_calibrateScannerSrv = create_service<std_srvs::srv::Trigger>("calibrate_scanner", std::bind(&SurfaceScannerNode::calibrateScannerSrvCallback, this, _1));

    m_calibrateLaserWithImportSrv = create_service<interfaces::srv::CalibrateLaserImport>("calibrate_with_import", std::bind(&SurfaceScannerNode::calibrateWithImportSrvCallback, this, _2));
    
    imgPairSub = create_subscription<interfaces::msg::ImagePair>("img_pair", 10, std::bind(&SurfaceScannerNode::imagePairCallback, this, _3));

    camCalibrateImgsSub = create_subscription<interfaces::msg::CameraCalibrationImgs>("cam_calib_imgs", 10, std::bind(&SurfaceScannerNode::camCalibImgsCallback, this, _4));

    pcdPub = create_publisher<sensor_msgs::msg::PointCloud2>("surface_line", 10);

    laserPlanePub = create_publisher<sensor_msgs::msg::PointCloud2>("laser_plane", 10);

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

        if(!m_Scanner.isScannerCalibrated()){
            response->success = false;
            response->message = "Scanner calibration unsuccessful!";
        }
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
        auto logger = this->get_logger();//rclcpp::get_logger("rclcpp");
        RCLCPP_INFO(logger, "Incoming request for scanner calibration with import of camera parameters!");
        RCLCPP_INFO(logger, "Holding two images to calibrate laser.");
        RCLCPP_INFO_STREAM(logger, "[origin_img: " << m_originImg.size() <<", laser_img: " << m_laserImg.size() << "]");
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
               RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), e.what());
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
                 RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), e.what());
            }
            // ToDo generate point cloud
        }
        else
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "Scanner is not calibrated!");      
    }
}

void SurfaceScannerNode::camCalibImgsCallback(const interfaces::msg::CameraCalibrationImgs & msg){
    for(const auto& img : msg.imgs){
        m_calibImgs.push_back(cv_bridge::toCvCopy(img, "bgr8")->image);
    }
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Recieved list with: "<<msg.imgs.size()<<" images for intrinsic camera calibration!");
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  std::shared_ptr<rclcpp::Node> node = std::make_shared<SurfaceScannerNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}