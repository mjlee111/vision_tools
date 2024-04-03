/**
 * @file mj_udp.hpp
 * @author mjlee111
 * @brief
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MJ_UDP_H
#define MJ_UDP_H
#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QMainWindow>
#include <QByteArray>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include "crc.h"
#include <thread>

using namespace cv;
using namespace std;

typedef struct
{
  cv::Mat img;
  int data_size;
} imgReturn;

class UDP : public QMainWindow
{
  Q_OBJECT

private:
public:
  UDP();
  ~UDP();

  bool connection = false;

  void udp_write_text(QString text, uint16_t port, QHostAddress address, QUdpSocket& socket);

  void udp_write_array(QByteArray text, uint16_t port, QHostAddress address, QUdpSocket& socket);

  bool crc8_check(const QByteArray& data);

  void crc8_input(QByteArray& data);

public slots:
  QByteArray udp_read(uint16_t port, QHostAddress& address, QUdpSocket& socket);

  bool udp_check_rcv(uint16_t port, QHostAddress& address, QUdpSocket& socket);

  void udp_check_trs(uint16_t port, QHostAddress& address, QUdpSocket& socket);

  void send_cam_img(vector<uchar> img, uint16_t port, QHostAddress& address, QUdpSocket& socket);

  imgReturn read_cam_img(cv::Mat img, uint16_t port, QHostAddress& address, QUdpSocket& socket);
};

#endif
