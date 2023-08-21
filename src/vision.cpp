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

    ui->fps_plot->setTitle("FPS");
    ui->fps_plot->setAxisTitle(QwtPlot::xBottom, "Time");
    fps_curve = new QwtPlotCurve("FPS");
    fps_curve->attach(ui->fps_plot);
    fps_curve->setPen(QColor(Qt::green));
    QPen pen = fps_curve->pen();
    pen.setWidth(2);
    fps_curve->setPen(pen);
    ui->fps_plot->setAxisScale(QwtPlot::xBottom, 0.0, 10.0);
    ui->fps_plot->setAxisScale(QwtPlot::yLeft, 0.0, 60.0);
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

    fps_timer->start(250);
    avg_rest_timer->start(1000);
    connect(fps_timer, SIGNAL(timeout()), this, SLOT(Fps_update()));
    connect(avg_rest_timer, SIGNAL(timeout()), this, SLOT(avg_reset()));
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

    if (cap.read(img))
    {
        cv::resize(img, img, cv::Size(320, 180));
        QImage raw_image((const unsigned char *)(img.data), img.cols,
                         img.rows, QImage::Format_RGB888);
        ui->raw_image->setPixmap(QPixmap::fromImage(raw_image.rgbSwapped()));
        // cal_size(img);
        fps++;
    }
}

void vision::Cam_update_udp()
{
    img = udpMJ->read_cam_img(img, port, Target_ip, *rcv_img);
    cv::resize(img, img, cv::Size(320, 180));
    QImage raw_image((const unsigned char *)(img.data), img.cols,
                     img.rows, QImage::Format_RGB888);
    ui->raw_image->setPixmap(QPixmap::fromImage(raw_image.rgbSwapped()));
    // cal_size(img);
    fps++;
}

void vision::Cam_update_ssh()
{
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
        // cal_size(img);
        fps++;
    }
}

void vision::Fps_update()
{
    fps = fps * 4;
    if (fps >= fps_min_max[1])
    {
        fps_min_max[1] = fps;
    }
    if (fps < fps_min_max[0])
    {
        fps_min_max[0] = fps;
    }
    ui->fps->setText(QString::number(fps));

    fps_curve_data.append(QPointF(xValue, fps));
    xValue += 0.25;

    QVector<double> xData, yData;
    for (const QPointF &point : fps_curve_data)
    {
        xData.append(point.x());
        yData.append(point.y());
    }
    if (xValue > 10.0)
    {
        ui->fps_plot->setAxisScale(QwtPlot::xBottom, 0.0, xValue + 10.0);
    }

    fps_curve->setSamples(xData, yData);
    ui->fps_plot->replot();
    ui->max_fps->setText(QString::number(fps_min_max[1]));
    ui->min_fps->setText(QString::number(fps_min_max[0]));
    fpsVector.push_back(fps);

    fps = 0;
}

void vision::avg_reset()
{
    double sum = 0.0;
    for (int i = 0; i < fpsVector.size(); ++i)
    {
        sum += fpsVector[i];
    }
    double avg = sum / fpsVector.size();
    ui->avg_fps->setText(QString::number(avg));
    fpsVector.clear();
}

void vision::cal_size(cv::Mat img)
{
    imageSizeBytes = img.total() * img.elemSize();
    bytesPerSecond = static_cast<double>(imageSizeBytes) * fps;
    ui->data_byte->setText(QString::number(imageSizeBytes));
}

vision::~vision()
{
    delete udpMJ;
    cap.release();
    cap2.release();
    delete ui;
}
