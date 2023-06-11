#include "Utility.hpp"

using smartCvMat = std::shared_ptr<cv::Mat>;
using smartLaserLine = std::shared_ptr<LaserLine>;


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
    LaserLine &operator=(const LaserLine &other);
    ~LaserLine();
    // getters
    cv::Mat get_laserPoints() const;
    cv::Mat get_rotMatrix() const;
    cv::Mat get_tVec() const;
};

LaserLine::LaserLine(cv::Mat originalImg, cv::Mat imgWithLaser, cv::Mat rotMat, cv::Mat tVec, cv::Mat mask): m_originalImg(originalImg), 
                    m_imgWithLaser(imgWithLaser),m_rotMat(rotMat), m_tVec(tVec), m_mask(mask), m_ImgDiffLaser(defaultMat), m_laserPoints(defaultMat)
{
    if(!mask.empty()){
        cv::Mat orgImg, imgLaser;

        cv::bitwise_and(originalImg, originalImg, orgImg, mask);
        cv::bitwise_and(imgWithLaser, imgWithLaser, imgLaser, mask);

        cv::subtract(imgLaser, orgImg, m_ImgDiffLaser);
        m_laserPoints = utility::getLinePixels(m_ImgDiffLaser);
    }
}

LaserLine::LaserLine(const LaserLine &other):m_originalImg(other.m_originalImg), m_imgWithLaser(other.m_imgWithLaser), m_rotMat(other.m_rotMat), m_mask(other.m_mask),
                       m_ImgDiffLaser(other.m_ImgDiffLaser), m_tVec(other.m_tVec), m_laserPoints(other.m_laserPoints)                
{

}
/*
LaserLine &LaserLine::operator=(LaserLine other)noexcept{
    std::swap(m_laserPoints, other.m_laserPoints);
    std::swap(m_ImgDiffLaser, other.m_ImgDiffLaser);
    std::swap(m_tVec, other.m_tVec);
    std::swap(m_originalImg, other.m_originalImg);
    std::swap(m_imgWithLaser, other.m_imgWithLaser);
    std::swap(m_rotMat, other.m_rotMat);
    std::swap(m_mask, other.m_mask);
    
    return *this;
}
*/
LaserLine::~LaserLine(){

}

cv::Mat LaserLine::get_laserPoints() const{
    return m_laserPoints;
}

cv::Mat LaserLine::get_rotMatrix() const{
    return m_rotMat;
    }

cv::Mat LaserLine::get_tVec() const{
    return m_tVec;
}

LaserLine &LaserLine::operator=(const LaserLine &other){
    m_ImgDiffLaser = other.m_ImgDiffLaser;
    m_tVec = other.m_tVec;
    m_laserPoints = other.m_laserPoints;
    m_originalImg = other.m_originalImg;
    m_imgWithLaser = other.m_imgWithLaser;
    m_rotMat = other.m_rotMat;
    m_mask = other.m_mask;

    return *this;
}
