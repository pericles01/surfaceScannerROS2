#pragma once
#include <opencv2/opencv.hpp>

//Load with parameters
//calibrate
//get_raw_image
//get_undistored_image
//get camera parameters

using namespace std;
using namespace cv;

class Camera {
public:
    Camera(const string& src = "None");
    
    ~Camera(){ cam.release(); }
    
    Mat get_raw_image();
    void calibrate_Camera(const std::vector<cv::Mat> &images, const bool &safe_data_in_npz=true);
    void importCameraParams(const string& src);
    Mat get_cam_mtx() const;
    Mat get_dist() const;
    Mat get_rot_mtx_at(int index) const;
    Mat get_tvec_at(int index) const;

    void set_savePath(const String& path){ m_savePath=path;};
    String get_savePath()const {return m_savePath;};
    
private:
    Mat m_cam_mtx;
    Mat m_dist;
    vector<Mat> v_rvecs;
    vector<Mat> v_tvecs;
    
    VideoCapture cam;
    Mat m_imageFrame;
    String m_savePath;
};


