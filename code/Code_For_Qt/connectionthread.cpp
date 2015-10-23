#include "connectionthread.h"
#include<QDebug>
#include<QMessageBox>


Connectthread::Connectthread(QObject *parent) :
    QThread(parent)
{
}
Connectthread::~Connectthread()
{
}
//启动adb server程序
void Connectthread::run()
{
    QProcess *phone=new QProcess(this);

    phone->start("/opt/ass/bin/adb kill-server");

    phone->start("/opt/ass/bin/adb start-server");
    phone->waitForFinished();

    qDebug()<<"connect thread is running";
    connect_change = new thread_2;
    connect_change->start();
    connect(connect_change,SIGNAL(connect_find(int)),this,SLOT(connect_success(int)));
   // emit connectionChanged(0,"");
}
void Connectthread::connect_success(int state)
{
    qDebug()<<"i have recv the message the connect is success";
    QProcess *phone=new QProcess(this);

     phone->start("/opt/ass/bin/adb forward tcp:12580 tcp:12580");
    phone->waitForFinished();

   // int ok = QMessageBox::about(NULL,tr(" delete"),tr("确定"),tr("取消"));

   // if(ok == QMessageBox::No)
  //  {
   // }
   // else
             emit connectionChanged(0,"");
   // disconnect(connect_change,SIGNAL(connect_find(int)),this,SLOT(connect_success(int)));
}
