#include <QApplication>
#include "../include/vision.h"
#include "../include/camera_set.h"
#include "ui_vision.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    camera_set cameraSetWindow;

    QObject::connect(&cameraSetWindow, &camera_set::cameraSetClosed, [&]()
                     {
        // Close the camera set window first
        cameraSetWindow.close();
        
        // Create and show the vision window
        vision *visionWindow = new vision;
        visionWindow->show(); });

    cameraSetWindow.show();

    return a.exec();
}
