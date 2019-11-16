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
    cv::threshold(img2, img2, BRIGHTNESS_THRESHOLD, 255, cv::THRESH_BINARY);
//    cv::blur(img2, img2, cv::Size(1,3));//均值滤波
}

void CvDetect::morphology(const cv::Mat& img1, cv::Mat& img2, int flag ){
    switch(flag){
        case 0:{//膨胀
            cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
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

void CvDetect::searchArmor(const cv::Mat& img1, cv::Mat& img2, int flag){
    std::vector<std::vector<cv::Point>> lightContours;//灯条边界
    switch (flag) {
    case 0:{
        std::vector<std::vector<cv::Point>> allContours;//提取出的所有边界（包含灯条）
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(img1.clone(), allContours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//找边界
        for(int i = 0; i < allContours.size(); i++){
            int childContourIndex = hierarchy[i][2];//获取子轮廓的index
            if(childContourIndex > 0){//如果有子轮廓
                while(childContourIndex > 0){
                    lightContours.push_back(allContours[childContourIndex]);//将子轮廓放进灯条轮廓候选
                    childContourIndex = hierarchy[childContourIndex][0];//该子轮廓的下个同级轮廓
                }
            }
        }
        //cv::drawContours(img2, lightContours, -1, cv::Scalar(0,255,0), 1);
        break;
    }
    case 1:{
        cv::findContours(img1.clone(), lightContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//找边界
        //cv::drawContours(img2, lightContours, -1, cv::Scalar(0,255,0), 1);
        break;
    }
    }
        //识别灯条
        std::vector<cv::RotatedRect> selectedRect;
        for(const auto& contour:lightContours){
            //筛掉太小的边缘
            float lightContourArea = cv::contourArea(contour);
            if(lightContourArea < _Armor.Light_Area_min||contour.size()<5)continue;

            //用椭圆拟合区域得到外接矩形
            cv::RotatedRect lightRec = cv::fitEllipse(contour);
            lightRec = _adjustRec(lightRec);//换成长边作为与x轴算夹角的边

            // 长宽比和轮廓面积比限制
            if (lightRec.size.width / lightRec.size.height > _Armor.Light_Aspect_ratio
                || lightContourArea / lightRec.size.area() < _Armor.Light_crown)
                continue;

            // 扩大灯柱的面积
            lightRec.size.height *= 1.1;
            lightRec.size.width *= 1.1;
            selectedRect.push_back(lightRec);

//            if (lightRec.angle > _Armor.Light_angle)
//                continue;



        }

        //识别装甲板
        std::vector<cv::RotatedRect> ArmorRect;
        for(int i = 0; i < selectedRect.size(); i++){
            for( int j = i + 1; j < selectedRect.size(); j++){
              //灯条匹配
                //判断是否为相同灯条
                float Contour_angle = abs(selectedRect[i].angle - selectedRect[j].angle); //角度差
                if (Contour_angle >= _Armor.Light_Contour_angle)
                    continue;
                //长度差比率
                float Contour_Len1 = std::abs(selectedRect[i].size.height - selectedRect[j].size.height) / std::max(selectedRect[i].size.height, selectedRect[j].size.height);
                //宽度差比率
                float Contour_Len2 = std::abs(selectedRect[i].size.width - selectedRect[j].size.width) / std::max(selectedRect[i].size.width, selectedRect[j].size.width);
                if (Contour_Len1 > _Armor.Light_Contour_Len || Contour_Len2 > _Armor.Light_Contour_Len)
                    continue;

              //装甲板筛选
                cv::RotatedRect Rect;
                Rect.center.x = (selectedRect[i].center.x + selectedRect[j].center.x) / 2.; //x坐标
                Rect.center.y = (selectedRect[i].center.y + selectedRect[j].center.y) / 2.; //y坐标
                Rect.angle = (selectedRect[i].angle + selectedRect[j].angle) / 2.; //角度
                float nh, nw, yDiff, xDiff;
                nh = (selectedRect[i].size.height + selectedRect[j].size.height) / 2; //高度
                // 宽度
                nw = sqrt((selectedRect[i].center.x - selectedRect[j].center.x) * (selectedRect[i].center.x - selectedRect[j].center.x) + (selectedRect[i].center.y - selectedRect[j].center.y) * (selectedRect[i].center.y - selectedRect[j].center.y));
                float ratio = nw / nh; //匹配到的装甲板的长宽比
                xDiff = abs(selectedRect[i].center.x - selectedRect[j].center.x) / nh; //x差比率
                yDiff = abs(selectedRect[i].center.y - selectedRect[j].center.y) / nh; //y差比率
                Rect = _adjustRec(Rect);
                if (Rect.angle > _Armor.Armor_angle_min || ratio < _Armor.Armor_ratio_min || ratio > _Armor.Armor_ratio_max || xDiff < _Armor.Armor_xDiff || yDiff > _Armor.Armor_yDiff)
                    continue;
                Rect.size.height = nh;
                Rect.size.width = nw;
                ArmorRect.push_back(Rect);

                //画出矩形
                for(int i = 0; i < ArmorRect.size(); i++){
                    _drawRotatedRect(img2, ArmorRect[i]);
                }
            }
        }
}

cv::RotatedRect& CvDetect::_adjustRec(cv::RotatedRect& rec)
{
    using std::swap;

    float& width = rec.size.width;
    float& height = rec.size.height;
    float& angle = rec.angle;


    while (angle >= 90.0) angle -= 180.0;
    while (angle < -90.0) angle += 180.0;



        if (angle >= 45.0)
        {
            swap(width, height);
            angle -= 90.0;
        }
        else if (angle < -45.0)
        {
            swap(width, height);
            angle += 90.0;
        }


    return rec;
}

cv::RotatedRect CvDetect::boundingRRect(const cv::RotatedRect &left, const cv::RotatedRect &right){
    const cv::Point & pl = left.center, & pr = right.center;
    cv::Point2f center = (pl + pr) / 2.0;
    cv::Size2f wh_l = left.size;
    cv::Size2f wh_r = right.size;
    float width = POINT_DIST(pl, pr) - (wh_l.width + wh_r.width) / 2.0;
    float height = std::max(wh_l.height, wh_r.height);
    //float height = (wh_l.height + wh_r.height) / 2.0;
    float angle = std::atan2(right.center.y - left.center.y, right.center.x - left.center.x);
    return cv::RotatedRect(center, cv::Size2f(width, height), angle * 180 / CV_PI);
}

void CvDetect::_drawRotatedRect(cv::Mat &img, const cv::RotatedRect &rec){
    //画出矩形
    cv::Point2f vertices[4];
    rec.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(img, vertices[i], vertices[(i+1)%4], cv::Scalar(0,255,0));
}

void CvDetect::_drawLightContours(cv::Mat& img, std::vector<std::vector<cv::Point>> lightContours){
    for(const auto& contour:lightContours){
        if(contour.size()<5)continue;//筛掉太小的边缘
        cv::RotatedRect lightRec = cv::fitEllipse(contour);
        cv::Rect lightRect = lightRec.boundingRect();
        cv::ellipse(img, lightRec, cv::Scalar(255,0,0));
        cv::rectangle(img, lightRect, cv::Scalar(0,255,0));
    }
}
