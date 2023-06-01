#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

const cv::Mat defaultMat = cv::Mat();//cv::Mat::zeros(cv::Size(3,3), CV_8U);

namespace utility{
    
    /*
        Extracting the laser points out of the difference image (surface_with_laser - surface).
        Creates and returns a list of subpixel.
    */
    cv::Mat getLinePixels(cv::InputArray diffImg){
        
    cv::Mat imgDiff;
    cv::cvtColor(diffImg, imgDiff, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(imgDiff, imgDiff, cv::Size(5, 5), 0.0);

    std::vector<double> x, y;

    // describes the deviation from the middle point
    //   [1, -1, 1],
        //    [0, 0, 1],
        //    [1, 1, 1]]
    cv::Mat mat = (cv::Mat_<double>(3, 3) << 1, -1, 1, 0, 0, 1, 1, 1, 1);

    double otsu_threshold = cv::threshold(imgDiff, imgDiff, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    std::cout << "INFO: calculated threshold is: " << otsu_threshold << std::endl;

    double row_index = 0.0;
    double max_intensity, x_coord, left_from_max_intensity, right_from_max_intensity, deviation_from_middle;
    cv::Mat rowMatrix, max_intensity_indices, intensity_vec;
    cv::Point max_intensity_index, intensity_vecintensity_vec;
    int max_intensity_row, max_intensity_col;
    for( int nbRow=0; nbRow<imgDiff.rows; ++nbRow){
            
            imgDiff.row(nbRow).copyTo(rowMatrix);
            // find the max value of the row array
            cv::minMaxLoc(rowMatrix, nullptr, &max_intensity);

            // apllying threshold
            if(max_intensity <= otsu_threshold){
                row_index += 1;
                continue;
            }

            cv::findNonZero(rowMatrix == max_intensity, max_intensity_indices);

            if (max_intensity_indices.total() > 1) {
                max_intensity_index = max_intensity_indices.at<cv::Point>(max_intensity_indices.total() / 2);
            } else {
                max_intensity_index = max_intensity_indices.at<cv::Point>(0);
            }

            max_intensity_row = max_intensity_index.y;
            max_intensity_col = max_intensity_index.x;

            
            if (max_intensity_col - 1 < 0) {
                left_from_max_intensity = max_intensity;
            } else {
                left_from_max_intensity = rowMatrix.at<double>(max_intensity_row, max_intensity_col - 1);
            }
            if (max_intensity_col + 1 == rowMatrix.cols) {
                right_from_max_intensity = max_intensity;
            } else {
                right_from_max_intensity = rowMatrix.at<double>(max_intensity_row, max_intensity_col + 1);
            }

            intensity_vec = (cv::Mat_<double>(1, 3) << left_from_max_intensity, max_intensity, right_from_max_intensity);
            
            if (intensity_vec.at<double>(0) == intensity_vec.at<double>(1) && intensity_vec.at<double>(1) == intensity_vec.at<double>(2)) {
                x_coord = max_intensity_col;
            } else {
                cv::Mat abc = mat.inv() * intensity_vec.t();
                deviation_from_middle = (-1 * abc.at<double>(1)) / (2 * abc.at<double>(0));
                x_coord = deviation_from_middle + max_intensity_col;
            }

            x.push_back(x_coord);
            y.push_back(row_index);
            row_index++;
        }

        std::vector<double> z(x.size(), 1.0);
        cv::Mat result;

        cv::hconcat(x, y, result);
        cv::hconcat(result, z, result);

        return result;
    }

    cv::Mat world2cam(const cv::Mat &pts, const cv::Mat &transVec, const cv::Mat &rotMatrix){
    return rotMatrix * pts + transVec;
    }

    cv::Mat cam2world(const cv::Mat &pts, const cv::Mat &transVec, const cv::Mat &rotMatrix){
        return rotMatrix.inv() * (pts - transVec);
    }

    /*
        Calculates the world coordinates out of given 2d pixel. Method needs to know the rotation matrix, translation vector
        of the world coordinate frame and the camera matrix.
        If a plane equation is given, the scale factor (s) is calculated with it. 
        Otherwise, the scaling factor (s) is calculated assuming that the z-coordinates of the points are 0.
    */
    cv::Mat bild2world(cv::Mat pts, cv::Mat transVec, cv::Mat rotMatrix, cv::Mat cam_matrix, cv::Mat plane=defaultMat){
        
        cv::Mat dirVec = rotMatrix.inv() * cam_matrix.inv() * pts;
        cv::Mat ptOnLine = rotMatrix.inv() * transVec;
        cv::Mat s, temp;

        if(!plane.empty()){
            cv::Mat n_pt = plane * ptOnLine;
            cv::Mat n_dir = plane * dirVec;
            s = (n_pt - plane.at<double>(3)) / n_dir;
        }
        else{
            dirVec.row(2).copyTo(temp);
            s = ptOnLine.at<double>(2) / temp;
        }

        return s * dirVec - ptOnLine;
    }

    /*
        Fits a plane in the given points using singular value decomposition.
        Calculates normal vector for the plane. Uses a random point from the given ones to complete the equation.
        Returns the plane equation like Ax + By + Cz + D = 0.
    */
    cv::Mat planeFitWithSvd(const cv::Mat &points){
        cv::Mat mean_points, random_point, plane;
        cv::reduce(points, mean_points, 1, cv::REDUCE_AVG);

        cv::Mat svd_u, svd_w, svd_vt;
        cv::SVDecomp(points - mean_points, svd_w, svd_u, svd_vt, cv::SVD::FULL_UV);
        svd_w.row(svd_w.rows -1).copyTo(plane);
        points.col(cv::theRNG().uniform(0, points.rows)).copyTo(random_point);

        cv::vconcat(plane, -random_point.dot(plane), plane);
        return plane;
    }
}