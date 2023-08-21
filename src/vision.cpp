#include "../include/vision.h"
#include "ui_vision.h"
#include <QString>

vision::vision(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::vision)
{
    ui->setupUi(this);
    QIcon icon("://images/camera.png");
    setWindowIcon(icon);

    std::cout << "SIBAL" << std::endl;

    cap.open(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 180);

    if (!cap.isOpened())
    {
        std::cout << "Cam Not Available" << std::endl;
        return;
    }

    QTimer *timer = new QTimer(this);
    QTimer *fps_timer = new QTimer(this);

    timer->start(33);
    fps_timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(Cam_update()));
    connect(fps_timer, SIGNAL(timeout()), this, SLOT(Fps_update()));
}

void vision::Cam_update()
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
