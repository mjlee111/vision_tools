#include <QApplication>
#include "../include/vision.h"
#include "../include/camera_set.h"
#include "ui_vision.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    camera_set cameraSetWindow;
    vision *visionWindow = new vision;

    QObject::connect(&cameraSetWindow, &camera_set::cameraSetClosed, [&]()
                     {
        cameraSetWindow.close();
        visionWindow->Start_stream();
        visionWindow->setFixedSize(770, 600);
        visionWindow->show(); });

    cameraSetWindow.setFixedSize(220, 205);
    cameraSetWindow.show();

    return a.exec();
}
