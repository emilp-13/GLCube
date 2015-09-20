#include "cube.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Cube w;
    w.show();

    return a.exec();
}
