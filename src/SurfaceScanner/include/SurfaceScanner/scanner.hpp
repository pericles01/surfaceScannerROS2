#pragma once
#include "SurfaceScanner/laser.hpp"
#include "SurfaceScanner/laserLine.hpp"
#include "SurfaceScanner/rpi_camera.hpp"
#include <opencv2/aruco.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <iostream>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>


/*
    Representation of the whole Scanner in one object.
    Needs camera and laser.
    Holds a point cloud as scan result.
*/
class Scanner {
public:
    Scanner();
    /*
        Method generates weighted color-values for every point of the Laser-Line by checking the original image (image
        without laser).
    */
    cv::Mat getPixelColors(const cv::Mat &pts_laser, const cv::Mat &img_original) const;
    bool isScannerCalibrated() const;
    std::tuple<bool, cv::Mat, cv::Mat, cv::Mat> get_pose_in_charuco_board(cv::Ptr<cv::aruco::DetectorParameters> aruco_params, cv::Ptr<cv::aruco::Board> board, cv::Ptr<cv::aruco::Dictionary> dictionary, const cv::Mat &img, const bool &primary) const;
    
    /*
        Method to calibrate the laser by finding the plane equation.
        Method assumes that the calibration images display the calibration charuco board.
        When no charuco board can be found the method will fail and returns false.
    */
    bool calibrateLaser(const cv::Mat &calibration_img, const cv::Mat &calibration_img_with_laser);

    /*
        Method to calibrate the whole scanner.
        Starting with intrinsic camera calibration. After that the laser will be calibrated.
    */
    void calibrateScanner(const std::vector<cv::Mat> &images, const cv::Mat &calibration_img, const cv::Mat &calibration_img_laser, const bool &safe_data_in_npz=true);
    
    /*
        Method to calibrate the whole scanner by importing the camera data and scipping the intrinsic calibration.
    */
    void calibrateScannerWithImport(const std::string &src, const cv::Mat &calibration_img, const cv::Mat &calibration_img_laser);

    /*
        Method to generate a point cloud out of the surface image pair given that the scanner is calibrated.
    */
    void generatePcd(const cv::Mat &surface_img, const cv::Mat &surface_img_laser);
    //ToDo
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr get_pcd() const;
    void updatePcd(const cv::Mat &surface_koords, const cv::Mat &point_colors);

private:
    Laser m_Laser;
    bool m_Calibrated;
    Camera m_Camera;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr m_CurrentSurface;

    /*
        Generates the surface koordinates using the plane equation.
    */
    void generateSurfaceLineKoordinates(const cv::Mat &surface_img, const cv::Mat &surface_img_laser, cv::Mat & surface_koords, cv::Mat & point_colors);
};