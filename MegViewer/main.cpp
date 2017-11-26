#include "megviewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MegViewer w;
    w.show();
    return a.exec();
}
