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
    Camera(const string& src);
    
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

Camera::Camera(const string& src = "None") {
	if (src != "None")
        importCameraParams(src);
}

void Camera::importCameraParams(const string& src){
    try{
		cv::FileStorage fs(src, cv::FileStorage::READ);
		fs["mtx"] >> m_cam_mtx;
		fs["dist"] >> m_dist;
		fs["rvecs"] >> v_rvecs;
		fs["tvecs"] >> v_tvecs;
    }
    catch(const cv::Exception &e){
        cerr<<e.what();
    }
}

Mat Camera::get_raw_image(){
	cam.open(0); 
	if (!cam.isOpened()){
		printf("Failed to connect to the camera!\n");
		return m_imageFrame;
	}
	
	cam.read(m_imageFrame);
	cam.release();
	return m_imageFrame;
}

void Camera::calibrate_Camera(const std::vector<cv::Mat> &images, const bool &safe_data_in_npz){
	int CHECKERBOARD[2]{6,9};

    vector<vector<Point3f>> objpoints;
    vector<vector<Point2f>> imgpoints;
    vector<Point3f> objp;
    for (int i = 0; i < CHECKERBOARD[1]; i++)
    {
        for (int j = 0; j < CHECKERBOARD[0]; j++)
            objp.push_back(Point3f(j, i, 0));
    }
    /*
    vector<String> images;
    string path = "./chessboard_images/*.png";
    glob(path, images);
    */
    Mat frame, gray;
    vector<Point2f> corner_pts;
    bool success;
    for (std::size_t i = 0; i < images.size(); i++)
    {
        success = findChessboardCorners(images[i], Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
        if (success)
        {
            TermCriteria criteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.001);
            cornerSubPix(gray, corner_pts, Size(11, 11), Size(-1, -1), criteria);
            drawChessboardCorners(frame, Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
            imwrite("./src/ros2_surface_scanner/surface_scanner/out/calibrated_" + to_string(i) + ".png", frame);
            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }
    }

    calibrateCamera(objpoints, imgpoints, gray.size(), m_cam_mtx, m_dist, v_rvecs, v_tvecs);
    
    if(safe_data_in_npz){
        assert((void("Warning!!! the save path is not defined yet! Use the Camera::set_savePath(path) method"), !m_savePath.empty()));
        FileStorage fs("./camera_params.npz", cv::FileStorage::WRITE);
        fs << "mtx" << m_cam_mtx;
        fs << "dist" << m_dist;
        fs << "rvecs" << v_rvecs;
        fs << "tvecs" << v_tvecs;
        fs.release();
        
        std::cout << "INFO: Camera-Params saved in 'camera_params.npz'!" << std::endl;  
    }

}

Mat Camera::get_cam_mtx()const
{
	return m_cam_mtx;
}

Mat Camera::get_dist() const
{
	return m_dist;
}

Mat Camera::get_rot_mtx_at(int index) const
{
	Mat rot;
	Rodrigues(v_rvecs[index], rot);
	return rot;
}

Mat Camera::get_tvec_at(int index) const
{
	return v_tvecs[index];
}



