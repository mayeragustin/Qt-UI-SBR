#include "qtuisbr.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtUISBR w;
    w.show();
    return a.exec();
}
