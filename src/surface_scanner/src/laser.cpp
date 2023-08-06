#include "surface_scanner/laser.hpp"

Laser::Laser(): m_up(defaultLaserLine), m_down(defaultLaserLine), m_points(defaultMat), m_planeEq(defaultMat){

}

cv::Mat Laser::get_planeEq() const{
    assert((void("Warning!!! Plane-equation is not defined yet!"), !m_planeEq.empty()));
    return m_planeEq;
}

void Laser::fitPlane(){
    m_planeEq = utility::planeFitWithSvd(m_points);
}

/*
    Finds the plane points out of the two laser lines.
    Merge tehm into one united coordinatesystem.
*/
void Laser::computePlanePoints(const cv::Mat &cameraMatrix){
    cv::Mat pts_up, pts_down, pts_down_cam, pts_down_up, temp;
    pts_up = utility::bild2world(m_up.get_laserPoints(), cameraMatrix,
                m_up.get_rotMatrix(), m_up.get_tVec());
        
    pts_down = utility::bild2world(m_down.get_laserPoints(), cameraMatrix,
            m_down.get_rotMatrix(), m_down.get_tVec());
    
    pts_down_cam = utility::world2cam(pts_down, m_down.get_tVec(),
            m_down.get_rotMatrix());
        
    pts_down_up = utility::cam2world(pts_down_cam, m_up.get_tVec(),
            m_up.get_rotMatrix());
    
    cv::hconcat(pts_up, pts_down_up, temp);
    cv::hconcat(m_points, temp, m_points);
}

/*
    Calulates the plane equation by first find the points and then fit the plane in it.
*/
void Laser::computePlaneEq(const cv::Mat &cameraMatrix){
    assert((void("Warning! Laser-lines are not defined!"), !m_up.get_laserPoints().empty() && !m_down.get_laserPoints().empty()));
    computePlanePoints(cameraMatrix);
    fitPlane();
}