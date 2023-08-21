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
#include <QString>
#include <QUdpSocket>
#include <QHostAddress>
#include "../include/mj_udp.h"

namespace Ui
{
    class vision;
}

class vision : public QMainWindow
{
    Q_OBJECT

public:
    vision(QWidget *parent = nullptr);
    ~vision();
    void Start_stream();
    bool Cam_init_usb(int cam_num);
    bool Cam_init_ssh();

    int usb_cam_num = 0;
    int mode = 0; // 1 = usb, 2 = udp 3 = ssh

    uint16_t port = 0;
    QString Sport;
    QString PC_address;
    QString Target_address;
    QHostAddress PC_ip;
    QHostAddress Target_ip;
    QUdpSocket *rcv_img = new QUdpSocket(this);

    QString Target_address_ssh;
    QString SSHport;
    std::string url;

private Q_SLOTS:
    void Cam_update_usb();
    void Cam_update_udp();
    void Cam_update_ssh();
    void Fps_update();

private:
    Ui::vision *ui;
    cv::VideoCapture cap;
    cv::VideoCapture cap2;

    UDP *udpMJ = nullptr;

    QTimer *timer = new QTimer(this);
    QTimer *timer2 = new QTimer(this);
    QTimer *fps_timer = new QTimer(this);

    int fps = 0;
};

#endif // VISION_H
