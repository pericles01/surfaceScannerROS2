#include "surface_scanner/scanner.hpp"


Scanner::Scanner(): m_Calibrated(false), m_Laser(Laser()), m_Camera(Camera()), m_CurrentSurface(new pcl::PointCloud<pcl::PointXYZRGB>()){

}

/*
    pts_laser ist a n*3 matrix
*/
cv::Mat Scanner::getPixelColors(const cv::Mat &pts_laser, const cv::Mat &img_original) const{
    
    cv::Mat color_values, pts;
    cv::Vec3f color_rounded_pix, color_next_pix, weighted_color;
    float decimal_place, decimal_place_reverse;

    for (int i = 0; i < pts_laser.rows; i++) {
        pts_laser.row(i).copyTo(pts);
        decimal_place = pts.at<double>(0) - pts.at<int>(0);
        decimal_place_reverse = 1 - decimal_place;

        color_rounded_pix = img_original.at<cv::Vec3f>(pts.at<int>(1), pts.at<int>(0)) / 255;

        if (pts.at<int>(0) >= img_original.cols - 1) {
            color_next_pix = color_rounded_pix;
        } else {
            color_next_pix = img_original.at<cv::Vec3f>(pts.at<double>(1), pts.at<double>(0) + 1) / 255;
        }

        weighted_color = (color_rounded_pix * decimal_place_reverse) + (color_next_pix * decimal_place);

        color_values.push_back(weighted_color);
    }

    return color_values; 
}

void Scanner::generateSurfaceLineKoordinates(const cv::Mat &surface_img, const cv::Mat &surface_img_laser, cv::Mat &surface_koords, cv::Mat &point_colors){
    assert((void("Warning!!! Plane-equation is not defined yet!"), !m_Laser.get_planeEq().empty()));
    cv::Mat cameraMatrix = m_Camera.get_cam_mtx();
    cv::Mat distCoeffs = m_Camera.get_dist();
    cv::Mat undistorted_img, undistorted_img_laser;

    cv::undistort(surface_img, undistorted_img, cameraMatrix, distCoeffs);
    cv::undistort(surface_img_laser, undistorted_img_laser, cameraMatrix, distCoeffs);
    
    LaserLine surfaceLine(undistorted_img, undistorted_img, m_Laser.get_up().get_rotMatrix(), m_Laser.get_up().get_tVec());

    cv::Mat points_surface = utility::bild2world(surfaceLine.get_laserPoints().t(), surfaceLine.get_tVec(), surfaceLine.get_rotMatrix(), cameraMatrix, m_Laser.get_planeEq());
    surface_koords = utility::world2cam(points_surface, surfaceLine.get_tVec(), surfaceLine.get_rotMatrix()); // 3*1 matrix
    point_colors = getPixelColors(surfaceLine.get_laserPoints().t(), surface_img); // 3*1 matrix
}   

bool Scanner::isScannerCalibrated() const{
    return m_Calibrated;
}


void Scanner::calibrateScanner(const std::vector<cv::Mat> &images, const cv::Mat &calibration_img, const cv::Mat &calibration_img_laser, const bool &safe_data_in_npz){
    m_Camera.calibrate_Camera(images, safe_data_in_npz);
    m_Calibrated = calibrateLaser(calibration_img, calibration_img_laser);
}

void Scanner::calibrateScannerWithImport(const std::string &src, const cv::Mat &calibration_img, const cv::Mat &calibration_img_laser){
    m_Camera.importCameraParams(src);
    m_Calibrated = calibrateLaser(calibration_img, calibration_img_laser);
}

std::tuple<bool, cv::Mat, cv::Mat, cv::Mat> Scanner::get_pose_in_charuco_board(cv::Ptr<cv::aruco::DetectorParameters> aruco_params, cv::Ptr<cv::aruco::Board> board, cv::Ptr<cv::aruco::Dictionary> dictionary, const cv::Mat &img, const bool &primary) const {
    bool control_flag = false;

    cv::Mat charuco_mask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);

    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<int> ids;
    std::vector<cv::Point2f> rejected_img_points;
    cv::aruco::detectMarkers(img, &dictionary, corners, ids, &aruco_params, rejected_img_points);

    if (!ids.empty()) {
        std::vector<cv::Point2f> charuco_corners;
        std::vector<int> charuco_ids;
        bool charuco_retval = cv::aruco::interpolateCornersCharuco(corners, ids, img, &board, charuco_corners, charuco_ids);

        if (!charuco_ids.empty()) {
            std::vector<std::vector<cv::Point>> hull(1);
            cv::convexHull(charuco_corners, hull[0]);
            cv::fillPoly(charuco_mask, hull, cv::Scalar(255));

            cv::Mat charuco_drawn = img.clone();
            cv::polylines(charuco_drawn, hull, true, cv::Scalar(0, 255, 255), 5);
            cv::aruco::drawDetectedCornersCharuco(charuco_drawn, charuco_corners, charuco_ids, cv::Scalar(0, 255, 0));

            if (primary)
                cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_convex_hull_primary.png", charuco_drawn);
            else
                cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_convex_hull_secondary.png", charuco_drawn);
        }

        cv::Mat rvec, tvec;
        cv::Mat cameraMatrix = m_Camera.get_cam_mtx();
        cv::Mat distCoeffs = m_Camera.get_dist();
        bool retval = cv::aruco::estimatePoseCharucoBoard(charuco_corners, charuco_ids, &board, cameraMatrix, distCoeffs, rvec, tvec);

        if (retval) {
            control_flag = true;

            cv::Mat rot_matrix;
            cv::Rodrigues(rvec, rot_matrix);

            return std::make_tuple(control_flag, rot_matrix, tvec, charuco_mask);
        } else {
            return std::make_tuple(control_flag, cv::Mat(), cv::Mat(), charuco_mask);
        }
    } else {
        return std::make_tuple(control_flag, cv::Mat(), cv::Mat(), charuco_mask);
    }
}

bool Scanner::calibrateLaser(const cv::Mat &calibration_img, const cv::Mat &calibration_img_with_laser){
    cv::Mat charuco_board = calibration_img;
    assert((void("Image at 'charuco_board' could not be loaded!"),!charuco_board.empty()));

    cv::Mat charuco_board_laser = calibration_img_with_laser;
    assert((void("Image at 'charuco_board_laser' could not be loaded!"),!charuco_board_laser.empty()));

    // Undistort calibration images
    cv::Mat cameraMatrix = m_Camera.get_cam_mtx();
    cv::Mat distCoeffs = m_Camera.get_dist();
    cv::Mat undistorted_board, undistorted_board_laser;
    cv::undistort(charuco_board, undistorted_board, cameraMatrix, distCoeffs);
    cv::undistort(charuco_board_laser, undistorted_board_laser, cameraMatrix, distCoeffs);

    // Generate output images
    cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_board.png", undistorted_board);
    cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_board_laser.png", undistorted_board_laser);

    // Create parameters for detecting the boards
    cv::Ptr<cv::aruco::DetectorParameters> aruco_params = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> aruco_dict_primary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::Ptr<cv::aruco::Dictionary> aruco_dict_secondary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_50);
    cv::Ptr<cv::aruco::CharucoBoard> board_primary = cv::aruco::CharucoBoard::create(7, 5, 0.025, 0.019, aruco_dict_primary);
    cv::Ptr<cv::aruco::CharucoBoard> board_secondary = cv::aruco::CharucoBoard::create(7, 5, 0.025, 0.019, aruco_dict_secondary);
    
    // Get pose of the first board
    std::tuple<bool, cv::Mat, cv::Mat, cv::Mat> poseFirstBoard = get_pose_in_charuco_board(aruco_params, board_primary, aruco_dict_primary, undistorted_board, true);
    if (!std::get<0>(poseFirstBoard)){
        std::cout<<"WARNING: Something went wrong with detecting the ChArUco-Board!"<<std::endl;
        return false;
    }

    // Get pose of the second board
    std::tuple<bool, cv::Mat, cv::Mat, cv::Mat> poseSecondBoard = get_pose_in_charuco_board(aruco_params, board_secondary, aruco_dict_secondary, undistorted_board, false);
    if (!std::get<0>(poseSecondBoard)){
        std::cout<<"WARNING: Something went wrong with detecting the ChArUco-Board!"<<std::endl;
        return false;
    }

    // Generate output images
    cv::Mat charuco_drawn_primary = undistorted_board.clone();
    cv::drawFrameAxes(charuco_drawn_primary, cameraMatrix, distCoeffs, std::get<1>(poseFirstBoard), std::get<2>(poseFirstBoard), 0.1);

    cv::Mat charuco_drawn_secondary = undistorted_board.clone();
    cv::drawFrameAxes(charuco_drawn_secondary, cameraMatrix, distCoeffs, std::get<1>(poseSecondBoard), std::get<2>(poseSecondBoard), 0.1);
    cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_primary.png", charuco_drawn_primary);
    cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_secondary.png", charuco_drawn_secondary);

    cv::Mat charuco_cut_primary, charuco_cut_secondary;
    cv::bitwise_and(undistorted_board_laser, undistorted_board_laser, charuco_cut_primary, std::get<3>(poseFirstBoard));
    cv::drawFrameAxes(charuco_cut_primary, cameraMatrix, distCoeffs, std::get<1>(poseFirstBoard), std::get<2>(poseFirstBoard), 0.1);
    cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_cut_primary.png", charuco_cut_primary);

    cv::bitwise_and(undistorted_board_laser, undistorted_board_laser, charuco_cut_secondary, std::get<3>(poseSecondBoard));
    cv::drawFrameAxes(charuco_cut_secondary, cameraMatrix, distCoeffs, std::get<1>(poseSecondBoard), std::get<2>(poseSecondBoard), 0.1);
    cv::imwrite("./src/ros2_surface_scanner/surface_scanner/out/extrinsic_calibration/charuco_cut_secondary.png", charuco_cut_secondary);

    // Fill in laser-line parameter in laser
    LaserLine up(charuco_board, charuco_board_laser, std::get<1>(poseFirstBoard), std::get<2>(poseFirstBoard), std::get<3>(poseFirstBoard));
    m_Laser.set_up(up);
    LaserLine down(charuco_board, charuco_board_laser, std::get<1>(poseSecondBoard), std::get<2>(poseSecondBoard), std::get<3>(poseSecondBoard));
    m_Laser.set_down(down);

    m_Laser.computePlaneEq(cameraMatrix);

    cv::Mat plane_eq = m_Laser.get_planeEq();
    std::cout << "INFO: Laser-plane calibrated with equation:\n" << plane_eq.at<double>(0) << " * X  +  " << plane_eq.at<double>(1) << " * Y  + " << plane_eq.at<double>(2) << " * Z  =  " << plane_eq.at<double>(3) << std::endl;
    
    return true;
}

void Scanner::generatePcd(const cv::Mat &surface_img, const cv::Mat &surface_img_laser){
    cv::Mat surface_koords, point_colors;
    generateSurfaceLineKoordinates(surface_img, surface_img_laser, surface_koords, point_colors);
    updatePcd(surface_koords.t(), point_colors);
    std::cout<<"INFO: Finished point cloud generation!"<<std::endl;
}

pcl::PointCloud<pcl::PointXYZRGB>::Ptr Scanner::get_pcd() const{
    return m_CurrentSurface;
}

/*
    takes two 3*1 or 1*3 matrixes as argument and update the point cloud
*/
void Scanner::updatePcd(const cv::Mat &surface_koords, const cv::Mat &point_colors){
    pcl::PointXYZRGB point;
    point.x = surface_koords.at<float>(0);
    point.y = surface_koords.at<float>(1);
    point.z = surface_koords.at<float>(2);
    point.r = point_colors.at<std::uint8_t>(0);
    point.g = point_colors.at<std::uint8_t>(1);
    point.b = point_colors.at<std::uint8_t>(2);
    
    m_CurrentSurface->push_back(point);
}