#include "thread_2.h"
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
extern "C"
{
#include<stdio.h>
#include<string.h>
}
thread_2::thread_2(QObject *parent) :
    QThread(parent)
{
}
void thread_2::run()//监控连接状态
{
    QProcess *connect_process = new QProcess;
    QString connect_out;
    connect_process->start("/opt/ass/bin/adb get-state");
    connect_process->waitForFinished();
    connect_out = connect_process->readAllStandardOutput();

    qDebug()<<connect_out;
    if(connect_out.contains("device"))
        emit connect_find(0) ;
    else
    {
        QProcess::execute("/opt/ass/bin/adb wait-for-device");
        emit connect_find(0);
     }
}
