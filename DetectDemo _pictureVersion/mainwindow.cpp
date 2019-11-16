#include "mainwindow.h"
#include "ui_mainwindow.h"


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

void MainWindow::on_pushButton_showOriginImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),
                                                    ".",
                                                    tr("Image File(*.png *.jpg *.jpeg *.bmp)"));

   if (fileName.isEmpty())
    {
        return;
    }
   _srcImage = cv::imread(fileName.toLatin1().data());//读取图片数据
   //cv::resize(_srcImage, _srcImage, cv::Size(350, 350));
   showOriginImg(_srcImage);
}

void MainWindow::on_pushButton_subtractChannel_clicked()
{
    cv::GaussianBlur(_srcImage, _processedImage, cv::Size(5, 5), 3, 3);
    _detector.subtractChannel(_processedImage, _processedImage);
   showProcessedImg(_processedImage);
}

void MainWindow::showOriginImg(const cv::Mat &img){
    cv::imshow("OringinImg", img);
    cv::waitKey(0);
}

void MainWindow::showProcessedImg(const cv::Mat &img){
    cv::imshow("ProcessedImg", img);
    cv::waitKey(0);
}
//void MainWindow::showOriginImg(const cv::Mat &imgShow){
//    cv::Mat imgShow_tmp;//用来在Qt上显示的
//    cv::cvtColor(imgShow, imgShow_tmp, cv::COLOR_BGR2RGB);//图像格式转换
//    QImage disImage = QImage((const unsigned char*)(imgShow_tmp.data),imgShow_tmp.cols,imgShow_tmp.rows, imgShow_tmp.cols*imgShow_tmp.channels(),  QImage::Format_RGB888);
//    QLabel* label = new QLabel();//QLabel放在scrollArea里面显示
//    label->setPixmap(QPixmap::fromImage(disImage));
//    label->resize(QSize(disImage.width(), disImage.height()));
//    ui->originImage->setWidget(label);
//}

//void MainWindow::showProcessedImg(const cv::Mat &imgShow){
//    cv::Mat imgShow_tmp;//用来在Qt上显示的
//    if (imgShow.channels()==1){
//        imgShow_tmp = imgShow;
//    }else if(imgShow.channels()==3){
//        cv::cvtColor(imgShow, imgShow_tmp, cv::COLOR_BGR2RGB);//图像格式转换
//    }
//    QImage disImage = QImage((const unsigned char*)(imgShow_tmp.data),imgShow_tmp.cols,imgShow_tmp.rows, imgShow_tmp.cols*imgShow_tmp.channels(),  QImage::Format_Grayscale8);
//    QLabel* label = new QLabel();//QLabel放在scrollArea里面显示
//    label->setPixmap(QPixmap::fromImage(disImage));
//    label->resize(QSize(disImage.width(), disImage.height()));
//    ui->processedImage->setWidget(label);
//}

void MainWindow::on_pushButton_clicked()
{
    _detector.HSVSegment(_srcImage, _processedImage, 1);
    showProcessedImg(_processedImage);
}

void MainWindow::on_pushButton_threshold_clicked()
{
    _detector.thresholdAndMorphology(_processedImage, _processedImage, 0);
    showProcessedImg(_processedImage);
}

void MainWindow::on_pushButton_searchLED_clicked()
{
    cv::Mat tmp;
    tmp = _srcImage.clone();
    _detector.searchLED(_processedImage, tmp);
    showProcessedImg(tmp);
}
