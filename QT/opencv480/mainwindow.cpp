#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cv::Mat image=cv::imread("E:\\project\\Resources\\images\\1.jpeg",cv::IMREAD_COLOR);//一定要使用绝对路径，其他可以回报错
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", image );
}

MainWindow::~MainWindow()
{
    delete ui;
}

