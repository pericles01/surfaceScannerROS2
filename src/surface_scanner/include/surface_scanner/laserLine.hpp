#pragma once
#include "utility.hpp"


/*
    Representation of a laser line.
    Needs two pictures of the same scene. One with laser and one without.
    Class can be initialized with a mask to use only a section of the images.
*/
class LaserLine
{
private:
    cv::Mat m_ImgDiffLaser, m_tVec, m_laserPoints;
    cv::Mat m_originalImg, m_imgWithLaser, m_rotMat, m_mask;
    //const cv::Mat defaultMatrix = cv::Mat::ones(cv::Size(1,1), CV_32FC3);
public:
    LaserLine(cv::Mat originalImg = defaultMat, cv::Mat imgWithLaser = defaultMat, cv::Mat rotMat = defaultMat, cv::Mat tVec = defaultMat, cv::Mat mask = defaultMat);
    // copy constructor
    LaserLine(const LaserLine &other);
    // copy assignment (copy-and-swap idiom)
    //LaserLine &operator=(LaserLine other)noexcept;
    LaserLine &operator=(const LaserLine& other);
    ~LaserLine();
    // getters
    cv::Mat get_laserPoints() const;
    cv::Mat get_rotMatrix() const;
    cv::Mat get_tVec() const;
};
