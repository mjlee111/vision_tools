#include "../include/mj_udp.h"

using namespace std;

UDP::UDP(void)
{
}

UDP::~UDP(void)
{
}

void UDP::udp_write_text(QString text, uint16_t port, QHostAddress address, QUdpSocket& socket)
{
  QByteArray packet;
  packet.append(text);
  socket.writeDatagram(packet, address, port);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  packet.clear();
}

void UDP::udp_write_array(QByteArray text, uint16_t port, QHostAddress address, QUdpSocket& socket)
{
  QByteArray packet;
  packet.append(text);
  socket.writeDatagram(packet, address, port);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  packet.clear();
}

bool UDP::crc8_check(const QByteArray& data)
{
  uint8_t end = data.at(data.size() - 1);
  QByteArray dataWithoutEnd = data;
  dataWithoutEnd.remove(dataWithoutEnd.size() - 1, 1);

  if (end != CRC8(true).calculate(dataWithoutEnd))
  {
    return false;
  }
  else
  {
    return true;
  }
  dataWithoutEnd.clear();
}

void UDP::crc8_input(QByteArray& data)
{
  uint8_t crc_result = CRC8(true).calculate(data);
  data.push_back(crc_result);
}

QByteArray UDP::udp_read(uint16_t port, QHostAddress& address, QUdpSocket& socket)
{
  QByteArray buffer;
  buffer.resize(socket.pendingDatagramSize());
  socket.readDatagram(buffer.data(), buffer.size(), &address, &port);
  /*

  put your return code here ...

  */
  return buffer;
}

bool UDP::udp_check_rcv(uint16_t port, QHostAddress& address, QUdpSocket& socket)
{
  QByteArray buffer;
  buffer.resize(socket.pendingDatagramSize());
  socket.readDatagram(buffer.data(), buffer.size(), &address, &port);

  if (buffer.at(0) == 'o' && buffer.at(1) == 'k')
  {
    QByteArray packet;
    packet.push_back("ok");
    socket.writeDatagram(packet, address, port);
    connection = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  else
  {
    QByteArray packet;
    packet.push_back("cl");  // connection lost
    socket.writeDatagram(packet, address, port);
    connection = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  buffer.clear();
  return connection;
}

void UDP::udp_check_trs(uint16_t port, QHostAddress& address, QUdpSocket& socket)
{
  QByteArray packet;
  packet.append("ok");
  socket.writeDatagram(packet, address, port);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  packet.clear();
}

void UDP::send_cam_img(vector<uchar> img, uint16_t port, QHostAddress& address, QUdpSocket& socket)
{
  QByteArray array;
  for (auto val : img)
  {
    array.push_back(val);
  }

  int sendChk;
  sendChk = socket.writeDatagram(array.data(), array.size(), address, port);
  while (sendChk == -1)
  {
    sendChk = socket.writeDatagram(array.data(), array.size(), address, port);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  array.clear();
}

imgReturn UDP::read_cam_img(cv::Mat img, uint16_t port, QHostAddress& address, QUdpSocket& socket)
{
  QByteArray cam_buffer;
  cam_buffer.resize(socket.pendingDatagramSize());
  socket.readDatagram(cam_buffer.data(), cam_buffer.size(), &address, &port);

  std::vector<uchar> down_decoding(cam_buffer.begin(), cam_buffer.end());
  img = imdecode(Mat(down_decoding), cv::IMREAD_COLOR);
  cam_buffer.clear();
  imgReturn instance;
  instance.img = img;
  instance.data_size = socket.pendingDatagramSize();

  return instance;
}
