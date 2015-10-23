#include "thread_1.h"
#include <QProcess>
#include<QDebug>
#include <dialog_1.h>
thread_1::thread_1(QObject *parent) :
    QThread(parent)
{
    stopped=false;
}
void thread_1::run()
{
    qreal i=0;
    while(!stopped)
    {
        //Dialog_1 *screen=new Dialog_1;
     //   screen->show();
        QProcess::execute("/opt/ass/bin/adb shell /system/bin/screencap -p /sdcard/screenshot.png");
        QProcess::execute("/opt/ass/bin/adb pull /sdcard/screenshot.png   /tmp/ass/screenshot.png");
      //  QProcess::execute("mv screenshot.png /tmp/ass/screenshot.png");
      //  qDebug()<<QString("in my thread ");
       // msleep(1000);
       // screen->deleteLater();
        emit picsignal(i);
        i++;
    }
    stopped=false;
}
void thread_1::stop()
{
    stopped=true;
}
