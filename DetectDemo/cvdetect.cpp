#include "cvdetect.h"


CvDetect::CvDetect()
{

}

void  CvDetect::subtractChannel(const cv::Mat& img1, cv::Mat& img2, int flag ){
    std::vector<cv::Mat> channels;
    cv::split(img1, channels);
    if(flag ==0 ){//red - blue
        img2 = channels[2] - channels[0];
    }else{
        img2 = channels[0] - channels[1];
    }
}

void CvDetect::thresholdAndMorphology(const cv::Mat& img1, cv::Mat& img2, int flag ){
    cv::threshold(img1, img2, BRIGHTNESS_THRESHOLD, 255, cv::THRESH_BINARY);
    switch(flag){
        case 0:{//腐蚀
            cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
            cv::dilate(img2, img2, element);
            break;
        }
        case 1:{//开操作
            cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(4, 4));
            cv::morphologyEx(img2, img2, cv::MORPH_OPEN, element);
            break;
        }
    }
}

void CvDetect::HSVSegment(const cv::Mat& img1, cv::Mat& img2, int flag ){
    if(flag == 1){

        cv::cvtColor(img1,img2,cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsvsplit;//对V进行直方图均衡
        cv::split(img2,hsvsplit);
        cv::equalizeHist(hsvsplit[2],hsvsplit[2]);
        cv::merge(hsvsplit,img2);

        #if ENERMY_COLOR //对面为蓝色
            cv::inRange(img2, cv::Scalar(B_LOW_H, B_LOW_S, B_LOW_V), cv::Scalar(B_HIGH_H, B_HIGH_S, B_HIGH_V), img2);
        #else//对面为红色
            cv::Mat tmpMask1, tmpMask2;
            cv::inRange(img2, cv::Scalar(R1_LOW_H, R_LOW_S, R_LOW_V), cv::Scalar(R1_HIGH_H, R_HIGH_S, R_HIGH_V), tmpMask1);
            cv::inRange(img2, cv::Scalar(R2_LOW_H, R_LOW_S, R_LOW_V), cv::Scalar(R2_HIGH_H, R_HIGH_S, R_HIGH_V), tmpMask2);
            img2 = tmpMask1 + tmpMask2;
        #endif
    }
}

void CvDetect::searchLED(const cv::Mat& img1, cv::Mat& img2){
    std::vector<std::vector<cv::Point>> lightContours;
    cv::findContours(img1.clone(), lightContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//找边界
    cv::drawContours(img2, lightContours, -1, cv::Scalar(0,255,0), 1);
}
