#ifndef VISION_H
#define VISION_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
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
#include <QDateTime>
#include <QVector>
#include <QPointF>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include "qwt/qwt_series_data.h"
#include <QPen>

#define CV_AA cv::LINE_AA
#define Mint cv::Scalar(208, 224, 64)

#define RAW_X 640
#define RAW_Y 480

namespace Ui
{
class vision;
}

class vision : public QMainWindow
{
  Q_OBJECT

public:
  vision(QWidget* parent = nullptr);
  ~vision();
  void Start_stream();
  bool Cam_init_usb(int cam_num);
  bool Cam_init_ssh();

  int usb_cam_num = 0;
  int mode = 0;  // 1 = usb, 2 = udp 3 = ssh

  uint16_t port = 0;
  QString Sport;
  QString PC_address;
  QString Target_address;
  QHostAddress PC_ip;
  QHostAddress Target_ip;
  QUdpSocket* rcv_img = new QUdpSocket(this);

  QString Target_address_ssh;
  QString SSHport;
  std::string url;
  double xValue = 0.0;

private Q_SLOTS:
  void Cam_update_usb();
  void Cam_update_udp();
  void Cam_update_ssh();
  void Fps_update();
  void avg_reset();

  void on_upper_H_valueChanged(int value);
  void on_upper_S_valueChanged(int value);
  void on_upper_V_valueChanged(int value);
  void on_lower_H_valueChanged(int value);
  void on_lower_S_valueChanged(int value);
  void on_lower_V_valueChanged(int value);

  void on_point1_x_valueChanged(int value);
  void on_point1_y_valueChanged(int value);
  void on_point2_x_valueChanged(int value);
  void on_point2_y_valueChanged(int value);
  void on_point3_x_valueChanged(int value);
  void on_point3_y_valueChanged(int value);
  void on_point4_x_valueChanged(int value);
  void on_point4_y_valueChanged(int value);

private:
  Ui::vision* ui;
  cv::VideoCapture cap;
  cv::VideoCapture cap2;

  cv::Mat img;
  cv::Mat birdeye;
  cv::Mat hsv_img;

  UDP* udpMJ = nullptr;

  QTimer* timer = new QTimer(this);
  QTimer* timer2 = new QTimer(this);
  QTimer* fps_timer = new QTimer(this);
  QTimer* avg_rest_timer = new QTimer(this);

  QwtPlotCurve* fps_curve;
  QVector<QPointF> fps_curve_data;

  double fps = 0;
  double fps_min_max[2] = { 100000, 0 };  // min , max
  std::vector<double> fpsVector;

  int imageSizeBytes = 0;
  double bytesPerSecond = 0.0;
  void cal_size(cv::Mat img);

  int hsv[6] = { 0, 0, 0, 0, 0, 0 };

  // 0 : Hue Max, 1 : Sat Max, 2 : Val Max

  int xy[8] = { 0, 0, 640, 0, 640, 480, 0, 480 };

  void birdeye_view(Mat& input_img, Mat& output_img);
  vector<Vec4i> cn_vec;
  void hsv_set(Mat& input_img, Mat& output_img);
};

#endif  // VISION_H
