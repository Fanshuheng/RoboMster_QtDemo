#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cvdetect.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QLabel>


#include "opencv2/opencv.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /*显示处理后的图片
    *img:显示图片
    */
    void showProcessedImg(const cv::Mat& img);//显示处理后的图片
    void showOriginImg(const cv::Mat& img);//显示处理前的图片

private slots:

    void on_pushButton_showOriginImage_clicked();

    void on_pushButton_subtractChannel_clicked();

    void on_pushButton_clicked();

    void on_pushButton_threshold_clicked();

    void on_pushButton_searchLED_clicked();

private:
    Ui::MainWindow *ui;
    cv::Mat _srcImage, _processedImage;//原始图片和已处理图片
    CvDetect _detector;//装甲板识别类
};
#endif // MAINWINDOW_H
