#pragma once
#include "SurfaceScanner/laserLine.hpp"

LaserLine defaultLaserLine = LaserLine();

/*
    Representation of the laser for the triangulation-sensor.
    Holds two laser lines to calibrate a plane equation. 
    Also holding the points for plane fitting and the plane equation itself after extrinsic calibration is finished.
*/
class Laser{
public:
    Laser();
    void set_up(LaserLine up){m_up = up;};
    LaserLine get_up() const{return m_up;};
    void set_down(LaserLine down){m_down = down;};
    LaserLine get_down() const{return m_down;};
    cv::Mat get_planeEq() const;
    void fitPlane();
    void computePlanePoints(const cv::Mat &cameraMatrix);
    void computePlaneEq(const cv::Mat &cameraMatrix);
private:
    LaserLine m_up, m_down;
    cv::Mat m_points, m_planeEq;


};