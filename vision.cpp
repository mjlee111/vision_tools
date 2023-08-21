#include "vision.h"
#include "./ui_vision.h"

vision::vision(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::vision)
{
    ui->setupUi(this);

    cap.open(0);

    if (!cap.isOpened())
    {
        std::cout << "Cam Not Available" << std::endl;
        return;
    }

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(Cam_update()));
    timer->start(100); // 30ms마다 타이머 동작
}

void vision::Cam_update()
{
    cap = cv::VideoCapture(0);
    cv::Mat frame;
    if (cap.read(frame))
    {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage qtImage = QImage((const unsigned char *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
        ui->raw_image->setPixmap(QPixmap::fromImage(qtImage).scaled(ui->raw_image->size(), Qt::KeepAspectRatio));
    }
}

vision::~vision()
{
    delete ui;
}
