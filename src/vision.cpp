#include "../include/vision.h"
#include "ui_vision.h"
#include <QString>

vision::vision(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::vision)
{
    ui->setupUi(this);
    QIcon icon("://images/camera.png");
    setWindowIcon(icon);
    udpMJ = new UDP;
}

void vision::Start_stream()
{
    switch (mode)
    {
    case 1:
        if (Cam_init_usb(usb_cam_num))
        {
            timer->start(33);
            connect(timer, SIGNAL(timeout()), this, SLOT(Cam_update_usb()));
        }
        else
        {
            return;
        }
        break;

    case 2:
        PC_ip = QHostAddress(PC_address);
        Target_ip = QHostAddress(Target_address);
        port = Sport.toUInt();
        if (rcv_img->bind(PC_ip, port, QUdpSocket::ShareAddress))
        {
            connect(rcv_img, SIGNAL(readyRead()), this, SLOT(Cam_update_udp()));
        }
        else
        {
            return;
        }
        break;
    case 3:
        url = "http://" + Target_address_ssh.toStdString() + ":" + SSHport.toStdString() + "/?action=stream";
        if (Cam_init_ssh())
        {
            timer2->start(33);
            connect(timer2, SIGNAL(timeout()), this, SLOT(Cam_update_ssh()));
        }
        else
        {
            return;
        }
        break;

    default:
        break;
    }

    fps_timer->start(1000);
    connect(fps_timer, SIGNAL(timeout()), this, SLOT(Fps_update()));
}

bool vision::Cam_init_usb(int cam_num)
{
    cap.open(cam_num);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 180);

    if (!cap.isOpened())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool vision::Cam_init_ssh()
{
    cap2 = cv::VideoCapture(url);
    if (cap2.isOpened())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void vision::Cam_update_usb()
{
    cv::Mat img;
    if (cap.read(img))
    {
        cv::resize(img, img, cv::Size(320, 180));
        QImage raw_image((const unsigned char *)(img.data), img.cols,
                         img.rows, QImage::Format_RGB888);
        ui->raw_image->setPixmap(QPixmap::fromImage(raw_image.rgbSwapped()));
        fps++;
    }
}

void vision::Cam_update_udp()
{
    cv::Mat img;
    img = udpMJ->read_cam_img(img, port, Target_ip, *rcv_img);
    cv::resize(img, img, cv::Size(320, 180));
    QImage raw_image((const unsigned char *)(img.data), img.cols,
                     img.rows, QImage::Format_RGB888);
    ui->raw_image->setPixmap(QPixmap::fromImage(raw_image.rgbSwapped()));
    fps++;
}

void vision::Cam_update_ssh()
{
    cv::Mat img;
    cap2 = cv::VideoCapture(url);
    if (!cap2.isOpened())
    {
        return;
    }
    if (cap2.read(img))
    {
        cv::resize(img, img, cv::Size(320, 180));
        QImage raw_image((const unsigned char *)(img.data), img.cols,
                         img.rows, QImage::Format_RGB888);
        ui->raw_image->setPixmap(QPixmap::fromImage(raw_image.rgbSwapped()));
        fps++;
    }
}

void vision::Fps_update()
{
    ui->fps->setText(QString::number(fps));
    fps = 0;
}

vision::~vision()
{
    delete udpMJ;
    cap.release();
    cap2.release();
    delete ui;
}
