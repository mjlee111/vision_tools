#include "vision.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    vision w;
    w.show();
    return a.exec();
}
