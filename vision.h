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
    int fps = 0;

private Q_SLOTS:
    void Cam_update();
    void Fps_update();

private:
    Ui::vision *ui;
    cv::VideoCapture cap;
};

#endif // VISION_H
