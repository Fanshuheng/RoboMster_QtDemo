#include "mainwindow.h"
#include "ui_mainwindow.h"

#define SEGMENT_METHOD 1//1:HSV;0:通道相减

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_start_clicked()
{
//    QString fileName = QFileDialog::getOpenFileName(           this,tr("Open Image"),
//                                                    ".",
//                                                    tr("All files (*.*);"));
    QString fileName = "/home/ch/vedio";

   if (fileName.isEmpty())
    {
        return;
    }
   cv::VideoCapture cap(fileName.toLatin1().data());//读取视频
   if (!cap.isOpened())return;
   while(1){
       //1.读取视频中的一帧
       cap >> _srcImage;
       if (_srcImage.empty())break;

       //2.通道相减或HSV分割
       #if SEGMENT_METHOD
       _detector.HSVSegment(_srcImage, _processedImage);
       #else
       cv::GaussianBlur(_srcImage, _processedImage, cv::Size(5, 5), 3, 3);
       _detector.subtractChannel(_processedImage, _processedImage);
       #endif
       //3.形态操作
       _detector.morphology(_processedImage, _processedImage, 0);
       //4.找装甲板
       cv::Mat tmp;
       tmp = _srcImage.clone();
       _detector.searchArmor(_processedImage, tmp, SEGMENT_METHOD);

       //显示
       cv::imshow("Origin", _srcImage);
        if(cv::waitKey()==27)break;//Esc
       cv::imshow("Processed", _processedImage);
        if(cv::waitKey()==27)break;//Esc
        cv::imshow("result", tmp);
         if(cv::waitKey()==27)break;//Esc
   }
}
