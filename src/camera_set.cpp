#include "../include/camera_set.h"
#include "ui_camera_set.h"
#include <QIcon>

camera_set::camera_set(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::camera_set)
{
    ui->setupUi(this);
    QIcon icon("://images/setting.png");
    setWindowIcon(icon);
}

camera_set::~camera_set()
{
    delete ui;
}

void camera_set::on_set_clicked()
{
    emit cameraSetClosed();
}

void camera_set::on_cancel_clicked()
{
    close();
    QApplication::exit(0);
}
