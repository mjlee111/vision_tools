#include "../include/vision.h"
#include "ui_vision.h"
#include <QString>

vision::vision(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::vision)
{
    ui->setupUi(this);
    QIcon icon("://images/camera.png");
    setWindowIcon(icon);
}

void vision::Start_stream()
{
    timer->start(33);
    fps_timer->start(1000);
    if (Cam_init_usb(usb_cam_num))
    {
        connect(timer, SIGNAL(timeout()), this, SLOT(Cam_update_usb()));
    }
    else
    {
        return;
    }
    connect(fps_timer, SIGNAL(timeout()), this, SLOT(Fps_update()));
}

bool vision::Cam_init_usb(int cam_num)
{
    cap.open(cam_num);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 180);

    if (!cap.isOpened())
    {
        std::cout << "Cam Not Available" << std::endl;
        return false;
    }
    else
    {
        std::cout << "Cam Initialized!" << std::endl;
        return true;
    }
}

void vision::Cam_update_usb()
{
    cv::Mat frame;
    if (cap.read(frame))
    {
        cv::resize(frame, frame, cv::Size(320, 180));
        QImage raw_image((const unsigned char *)(frame.data), frame.cols,
                         frame.rows, QImage::Format_RGB888);
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
    cap.release(); // Release the VideoCapture
    delete ui;
}
