#include "recording.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "Start";

    Recording rec;
    rec.show();

    while(true)
    {
        QCoreApplication::processEvents();
        rec.shootScreen();
        QThread::msleep(100);
    }

    return a.exec();
}
