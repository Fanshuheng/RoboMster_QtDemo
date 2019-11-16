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

//#define B_LOW_H   60
//#define B_HIGH_H  120
//#define B_LOW_S   120
//#define B_HIGH_S  255
//#define B_LOW_V   245
//#define B_HIGH_V  255

#define B_LOW_H   60
#define B_HIGH_H  120
#define B_LOW_S   15
#define B_HIGH_S  45
#define B_LOW_V   235
#define B_HIGH_V  255

#define W_LOW_H   0
#define W_HIGH_H  180
#define W_LOW_S   0
#define W_HIGH_S  30
#define W_LOW_V   220
#define W_HIGH_V  255

#define BRIGHTNESS_THRESHOLD 50 //二值化预阈值
#define POINT_DIST(p1,p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y))

struct ArmorParam {
    float Image_bright; // 图像降低的亮度
    int threshold_value; // threshold阈值

    float Light_Area_min; // 灯柱的最小值
    float Light_Aspect_ratio; // 灯柱的长宽比限制
    float Light_crown; // 灯柱的轮廓面积比限制

    float Light_angle; // 灯柱的倾斜角度
    float Light_Contour_angle; // 灯柱角度差
    float Light_Contour_Len; // 灯柱长度差比率

    float Armor_ratio_max; // 装甲板的长宽比max
    float Armor_ratio_min; // 装甲板的长宽比min
    float Armor_xDiff; // 装甲板x差比率
    float Armor_yDiff; // 装甲板y差比率
    float Armor_angle_min; // 装甲板角度min
    bool Armor_Color; // 装甲板颜色
    ArmorParam()
    {
        Image_bright = -100;
        threshold_value = 25;
        Light_Area_min = 20;
        Light_Aspect_ratio = 0.7;
        Light_crown = 0.5;
        Light_angle = 15;
        Light_Contour_angle = 4.2;
        Light_Contour_Len = 0.25;
        Armor_ratio_max = 5.0;
        Armor_ratio_min = 1.0;
        Armor_xDiff = 0.5;
        Armor_yDiff = 2.0;
        Armor_angle_min = 5;
    }
};

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

    /* 形态操作
     * img1:原图像
     * img2:处理后图像
     * flag = 0: 膨胀
     * flag = 1: 开操作
    */
    void morphology(const cv::Mat& img1, cv::Mat& img2, int flag = 0);

    /* 查找装甲板
     * img1:原图像
     * img2:查找后要显示出来的图像
     * flag = 0: 通道相减
     * flag = 1: HSV分割
    */
    void searchArmor(const cv::Mat& img1, cv::Mat& img2, int flag = 0);

    /**
     * @brief boundingRRect Bounding of two ratate rectangle (minumum area that contacts two inputs)
     * @param left left RotatedRect
     * @param right right RotatedRect
     * @return minumum area that contacts two inputs
     */
    cv::RotatedRect boundingRRect(const cv::RotatedRect & left, const cv::RotatedRect & right);

private:
    /* 使矩形换成用长边作为与width边
     * rec:原矩形
    */
    cv::RotatedRect& _adjustRec(cv::RotatedRect& rec);

    /* 画出RotatedRect
     * img: 要在上面显示RotatedRect的图像
     * rec: 要画出的RotatedRec
    */
    void _drawRotatedRect(cv::Mat& img, const cv::RotatedRect& rec);

    /* 画出contour拟合的椭圆和外接矩形
     * img: 要在上面显示lightContours的图像
     * lightContours: 要画的边界
    */
    void _drawLightContours(cv::Mat& img, std::vector<std::vector<cv::Point>> lightContours);

    ArmorParam _Armor;
};

#endif // CVDETECT_H
