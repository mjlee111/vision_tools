#ifndef camera_set_H
#define camera_set_H

#include <QMainWindow>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QWidget>
#include <QImage>
#include <QString>

namespace Ui
{
    class camera_set;
}

class camera_set : public QMainWindow
{
    Q_OBJECT

public:
    camera_set(QWidget *parent = nullptr);
    ~camera_set();

private Q_SLOTS:
    void on_set_clicked();
    void on_cancel_clicked();

signals:
    void cameraSetClosed();

private:
    Ui::camera_set *ui;
};

#endif // camera_set_H
