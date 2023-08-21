#ifndef VISION_H
#define VISION_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QWidget>
#include <QImage>

QT_BEGIN_NAMESPACE
using namespace cv;

namespace Ui
{
    class vision;
}
QT_END_NAMESPACE

class vision : public QMainWindow
{
    Q_OBJECT

public:
    vision(QWidget *parent = nullptr);
    ~vision();

    cv::VideoCapture cap;
    cv::Mat img;

public Q_SLOTS:
    void Cam_update();

private:
    Ui::vision *ui;
};
#endif // VISION_H
