#include "senderwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SenderWidget w;
    w.show();

    return a.exec();
}
