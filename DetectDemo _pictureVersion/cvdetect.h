#ifndef CVDETECT_H
#define CVDETECT_H

#include "opencv2/opencv.hpp"

#define ENERMY_COLOR  1//1:对面为蓝色；0：对面为红色

//HSV分割范围
#define R1_LOW_H   0
#define R1_HIGH_H  10
#define R2_LOW_H   156
#define R2_HIGH_H  180
#define R_LOW_S    43
#define R_HIGH_S   255
#define R_LOW_V    46
#define R_HIGH_V   255

//#define B_LOW_H   100
//#define B_HIGH_H  124
//#define B_LOW_S   0
//#define B_HIGH_S  255
//#define B_LOW_V   245
//#define B_HIGH_V  255

#define B_LOW_H   60
#define B_HIGH_H  120
#define B_LOW_S   120
#define B_HIGH_S  255
#define B_LOW_V   245
#define B_HIGH_V  255

#define BRIGHTNESS_THRESHOLD 50 //二值化预阈值

class CvDetect
{
public:
    CvDetect();

    /* 通道相减
     * img1:原图像
     * img2:图像通道相减后图像
     * flag = 0: 红-蓝 ；flag = 1 蓝 - 红
    */
    void subtractChannel(const cv::Mat& img1, cv::Mat& img2, int flag = ENERMY_COLOR );

    /* HSV分割
     * img1:原图像
     * img2:图像通道相减后图像
     * flag = 0: 分割出红色 ；flag = 1 分割出蓝色
    */
    void HSVSegment(const cv::Mat& img1, cv::Mat& img2, int flag = ENERMY_COLOR );

    /* 二值化加形态操作
     * img1:原图像
     * img2:处理后图像
     * flag = 0: 膨胀
    */
    void thresholdAndMorphology(const cv::Mat& img1, cv::Mat& img2, int flag = 0);

    /* 形态操作
     * img1:原图像
     * img2:处理后图像
     * flag = 0: 膨胀
     * flag = 1: 开操作
    */
    void morphology(const cv::Mat& img1, cv::Mat& img2, int flag = 0);

    void searchLED(const cv::Mat& img1, cv::Mat& img2);
};

#endif // CVDETECT_H
