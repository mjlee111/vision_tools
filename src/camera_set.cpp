#include "../include/camera_set.h"
#include "ui_camera_set.h"
#include <QIcon>

camera_set::camera_set(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::camera_set)
{
    ui->setupUi(this);
    QIcon icon("://images/setting.png");
    setWindowIcon(icon);
    ui->pc_ip->setPlaceholderText("192.168.1.1");
    ui->target_ip->setPlaceholderText("192.168.1.100");
    ui->port->setPlaceholderText("8888");
    ui->target_ip_ssh->setPlaceholderText("192.168.1.1");
    ui->port_ssh->setPlaceholderText("8090");

    usb_cam_grep();
}

void camera_set::usb_cam_grep()
{
    std::vector<int> availableCameras;
    for (int i = 0; i < 10; ++i)
    {
        cv::VideoCapture cap(i);
        if (cap.isOpened())
        {
            availableCameras.push_back(i);
            cap.release();
        }
    }

    for (int cameraIndex : availableCameras)
    {
        cameraList.append(QString("/dev/video%1").arg(cameraIndex));
    }

    QStringListModel *model = new QStringListModel(cameraList, this);
    ui->usb_cam_list->setModel(model);
}

camera_set::~camera_set()
{
    delete ui;
}

void camera_set::on_set_clicked()
{
    int cur_tab = ui->tabWidget->currentIndex();
    if (cur_tab == 0) // usb cam mode
    {
        QModelIndex selectedIndex = ui->usb_cam_list->currentIndex();
        if (selectedIndex.isValid())
        {
            int selectedRow = selectedIndex.row();
            _vision->usb_cam_num = selectedRow;
        }
        else
        {
            QMessageBox::warning(this, "WARNING", "YOU NEED TO SELECT A CAMERA!");
            return;
        }
        _vision->mode = 1;
    }
    else if (cur_tab == 1)
    {
        QString as = ui->pc_ip->toPlainText();
        QString bs = ui->target_ip->toPlainText();
        QString cs = ui->port->toPlainText();

        if (as.isEmpty() || bs.isEmpty() || cs.isEmpty())
        {
            QMessageBox::warning(this, "WARNING", "YOU NEED TO ENTER ALL THE DATA!");
            return;
        }
        else if (!as.isEmpty() && !bs.isEmpty() && !cs.isEmpty())
        {
            _vision->PC_address = as;
            _vision->Target_address = bs;
            _vision->Sport = cs;
        }
        _vision->mode = 2;
    }
    else if (cur_tab == 2)
    {
        QString as = ui->target_ip_ssh->toPlainText();
        QString bs = ui->port_ssh->toPlainText();

        if (as.isEmpty() || bs.isEmpty())
        {
            QMessageBox::warning(this, "WARNING", "YOU NEED TO ENTER ALL THE DATA!");
            return;
        }
        else if (!as.isEmpty() && !bs.isEmpty())
        {
            _vision->Target_address_ssh = as;
            _vision->SSHport = bs;
        }
        _vision->mode = 3;
    }

    emit cameraSetClosed();
    _vision->Start_stream();
    _vision->setFixedSize(770, 600);
    _vision->show();
}

void camera_set::on_cancel_clicked()
{
    close();
    QApplication::exit(0);
}
