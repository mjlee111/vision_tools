#include "../include/vision.h"
#include "ui_vision.h"
#include <QString>

vision::vision(QWidget* parent) : QMainWindow(parent), ui(new Ui::vision)
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

  ui->point1_x->setMaximum(RAW_X);
  ui->point2_x->setMaximum(RAW_X);
  ui->point3_x->setMaximum(RAW_X);
  ui->point4_x->setMaximum(RAW_X);

  ui->point1_y->setMaximum(RAW_Y);
  ui->point2_y->setMaximum(RAW_Y);
  ui->point3_y->setMaximum(RAW_Y);
  ui->point4_y->setMaximum(RAW_Y);

  ui->point1_x_val->setText(QString::number(xy[0]));
  ui->point1_y_val->setText(QString::number(xy[1]));
  ui->point2_x_val->setText(QString::number(xy[2]));
  ui->point2_y_val->setText(QString::number(xy[3]));
  ui->point3_x_val->setText(QString::number(xy[4]));
  ui->point3_y_val->setText(QString::number(xy[5]));
  ui->point4_x_val->setText(QString::number(xy[6]));
  ui->point4_y_val->setText(QString::number(xy[7]));

  ui->point1_x->setValue(xy[0]);
  ui->point1_y->setValue(xy[1]);
  ui->point2_x->setValue(xy[2]);
  ui->point2_y->setValue(xy[3]);
  ui->point3_x->setValue(xy[4]);
  ui->point3_y->setValue(xy[5]);
  ui->point4_x->setValue(xy[6]);
  ui->point4_y->setValue(xy[7]);
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
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

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
    // hsv_set(img, hsv_img);
    birdeye_view(img, birdeye);
    fps++;
  }
}

void vision::Cam_update_udp()
{
  imgReturn instance;
  instance = udpMJ->read_cam_img(img, port, Target_ip, *rcv_img);
  birdeye_view(instance.img, birdeye);
  ui->data_byte->setText(QString::number(instance.data_size));
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
    birdeye_view(img, birdeye);
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
  for (const QPointF& point : fps_curve_data)
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

void vision::birdeye_view(Mat& input_img, Mat& output_img)
{
  Point2f inputQuad[4], outputQuad[4];

  inputQuad[0] = Point2f(xy[0], xy[1]);
  inputQuad[1] = Point2f(xy[2], xy[3]);
  inputQuad[2] = Point2f(xy[4], xy[5]);
  inputQuad[3] = Point2f(xy[6], xy[7]);

  outputQuad[0] = Point2f(0, 0);
  outputQuad[1] = Point2f(RAW_X, 0);
  outputQuad[2] = Point2f(RAW_X, RAW_Y);
  outputQuad[3] = Point2f(0, RAW_Y);

  putText(input_img, "0", inputQuad[0], 1, 2, Mint, 1, 8);
  putText(input_img, "1", inputQuad[1], 1, 2, Mint, 1, 8);
  putText(input_img, "2", inputQuad[2], 1, 2, Mint, 1, 8);
  putText(input_img, "3", inputQuad[3], 1, 2, Mint, 1, 8);

  Mat M = getPerspectiveTransform(inputQuad, outputQuad);
  warpPerspective(input_img, output_img, M, output_img.size(), INTER_LINEAR, BORDER_CONSTANT);

  circle(input_img, inputQuad[0], 3, Mint, -1);
  circle(input_img, inputQuad[1], 3, Mint, -1);
  circle(input_img, inputQuad[2], 3, Mint, -1);
  circle(input_img, inputQuad[3], 3, Mint, -1);

  line(input_img, inputQuad[0], inputQuad[3], Mint, 2, CV_AA);
  line(input_img, inputQuad[1], inputQuad[2], Mint, 2, CV_AA);

  cv::resize(input_img, input_img, cv::Size(320, 240));
  cv::resize(output_img, output_img, cv::Size(320, 240));

  QImage be_image((const unsigned char*)(output_img.data), output_img.cols, output_img.rows, QImage::Format_RGB888);
  QImage raw_image((const unsigned char*)(input_img.data), input_img.cols, input_img.rows, QImage::Format_RGB888);
  ui->raw_image->setPixmap(QPixmap::fromImage(raw_image.rgbSwapped()));
  ui->birdeye_image->setPixmap(QPixmap::fromImage(be_image.rgbSwapped()));
}

void vision::hsv_set(Mat& input_img, Mat& output_img)
{
  std::cout << "r" << std::endl;
  Mat mask1 =
      getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));  // 지정 크기 만큼 모폴로지 연산/1,1은 디폴트 값.
  output_img = input_img.clone();
  GaussianBlur(output_img, output_img, Size(15, 15), 2.0);
  cvtColor(output_img, output_img, cv::COLOR_BGR2HSV);
  inRange(output_img, Scalar(hsv[0], hsv[1], hsv[2]), Scalar(hsv[3], hsv[4], hsv[5]), output_img);
  erode(output_img, output_img, mask1, Point(-1, -1), 2);
  dilate(output_img, output_img, mask1, Point(-1, -1), 2);
  cv::resize(output_img, output_img, cv::Size(320, 240));
  QImage hsv_image((const unsigned char*)(output_img.data), output_img.cols, output_img.rows, QImage::Format_RGB888);
  ui->hsv_image->setPixmap(QPixmap::fromImage(hsv_image.rgbSwapped()));
}

void vision::on_upper_H_valueChanged(int value)
{
  if (value < hsv[3])
  {
    ui->upper_H->setValue(hsv[3]);
    hsv[0] = hsv[3];
  }
  else
  {
    hsv[0] = value;
  }
  ui->label_upper_H->setText(QString::number(hsv[0]));
}

void vision::on_upper_S_valueChanged(int value)
{
  if (value < hsv[4])
  {
    ui->upper_S->setValue(hsv[4]);
    hsv[1] = hsv[4];
  }
  else
  {
    hsv[1] = value;
  }
  ui->label_upper_S->setText(QString::number(hsv[1]));
}

void vision::on_upper_V_valueChanged(int value)
{
  if (value < hsv[5])
  {
    ui->upper_V->setValue(hsv[5]);
    hsv[2] = hsv[5];
  }
  else
  {
    hsv[2] = value;
  }
  ui->label_upper_V->setText(QString::number(hsv[2]));
}

void vision::on_lower_H_valueChanged(int value)
{
  if (value >= hsv[0])
  {
    ui->lower_H->setValue(hsv[0]);
    hsv[3] = hsv[0];
  }
  else
  {
    hsv[3] = value;
  }
  ui->label_lower_H->setText(QString::number(hsv[3]));
}

void vision::on_lower_S_valueChanged(int value)
{
  if (value >= hsv[1])
  {
    ui->lower_S->setValue(hsv[1]);
    hsv[4] = hsv[1];
  }
  else
  {
    hsv[4] = value;
  }
  ui->label_lower_S->setText(QString::number(hsv[4]));
}

void vision::on_lower_V_valueChanged(int value)
{
  if (value >= hsv[2])
  {
    ui->lower_V->setValue(hsv[2]);
    hsv[5] = hsv[2];
  }
  else
  {
    hsv[5] = value;
  }
  ui->label_lower_V->setText(QString::number(hsv[5]));
}

void vision::on_point1_x_valueChanged(int value)
{
  xy[0] = value;
  ui->point1_x_val->setText(QString::number(xy[0]));
}

void vision::on_point1_y_valueChanged(int value)
{
  xy[1] = value;
  ui->point1_y_val->setText(QString::number(xy[1]));
}

void vision::on_point2_x_valueChanged(int value)
{
  xy[2] = value;
  ui->point2_x_val->setText(QString::number(xy[2]));
}

void vision::on_point2_y_valueChanged(int value)
{
  xy[3] = value;
  ui->point2_y_val->setText(QString::number(xy[3]));
}

void vision::on_point3_x_valueChanged(int value)
{
  xy[4] = value;
  ui->point3_x_val->setText(QString::number(xy[4]));
}

void vision::on_point3_y_valueChanged(int value)
{
  xy[5] = value;
  ui->point3_y_val->setText(QString::number(xy[5]));
}

void vision::on_point4_x_valueChanged(int value)
{
  xy[6] = value;
  ui->point4_x_val->setText(QString::number(xy[6]));
}

void vision::on_point4_y_valueChanged(int value)
{
  xy[7] = value;
  ui->point4_y_val->setText(QString::number(xy[7]));
}

vision::~vision()
{
  delete udpMJ;
  cap.release();
  cap2.release();
  delete ui;
}
