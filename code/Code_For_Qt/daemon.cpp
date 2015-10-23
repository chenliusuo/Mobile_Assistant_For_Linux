#include "daemon.h"
#include"assevent.h"
#include"recvthread.h"
#include"connectionthread.h"
#include<QDebug>
#include<QMessageBox>
#include<unistd.h>
extern "C"
{
 #include"package_info.h"
}

daemon::daemon(QWidget* objwidget) :
    MainWidget((QObject*)objwidget)
{
}
daemon::~daemon(){
}
//监测mainwindow发送过来的事件
void daemon::customEvent(QEvent* e){
    ASSEvent* ame = dynamic_cast<ASSEvent*>(e);
    switch (ame->type()){
        case ASSEVENT_DAEMON:
            recv_daemoninit();
            break;
        default:
            break;
    }
}
//mainwindow执行daemonthreadinit之后，进行初始化
void daemon:: recv_daemoninit()
{
    Connecttestinit();
    recv_threadinit();
    connect(MainWidget,SIGNAL(device_offline()),this,SLOT(mms_over()));
}

void daemon::mms_over()
{
   qDebug()<<"mmsthread run is over";
    mmstype->stop();
}
//打开adb-server
void daemon::Connecttestinit()
{
    connecttest = new Connectthread;
    connecttest->start();
    connect(connecttest, SIGNAL(connectionChanged(int,QString)), this, SLOT(slotConnectionChanged(int,QString)));
}
//监测手机是否安装，并对ｃ端套接字以及接收发送线程进行初始化
void daemon::slotConnectionChanged(int ,QString)
{
    int app_result = checkapp();
    qDebug()<<app_result;
    if(app_result >=1 )
    {
         main_pthread_init();
         qDebug()<<"main_thread_init is running";
         mmstype = new Mmsthread;
         mmstype->start();
         emit connect_finished();
    }
}

//监测手机是否安装，如果没有安装，自动给手机安装
int daemon::checkapp()
{
    QProcess *phone = new QProcess;
    QString outputlines;

    phone->start("/opt/ass/bin/adb shell pm list packages | grep com.android.ass");
    phone->waitForFinished(30000);
    outputlines = phone->readAll();

    if(outputlines.contains("com.android.ass"))
    {
        qDebug()<<"ass is find   ";
         phone->start("/opt/ass/bin/adb shell am start -n com.android.ass/.AssActivity");
         phone->waitForFinished(30000);

         outputlines="";
        return 1;
   }
    else
  {
       phone->start("/opt/ass/bin/adb install /opt/ass/bin/Ass.apk");
       phone->waitForFinished(30000);

        outputlines = phone->readAllStandardOutput();
        qDebug()<<outputlines;
        if(outputlines.contains("Success"))
        {
                 phone->start("/opt/ass/bin/adb shell am start -n com.android.ass/.AssActivity");
                 phone->waitForFinished();
                 outputlines = "";
                return 1;
        }
        else
            return 0;
    }

}
//初始化接收线程，让接收线程循环等待
void daemon::recv_threadinit()
{
    RecvThread = new recv_thread;
    RecvThread->start();
    connect(RecvThread,SIGNAL(recv_signals(int)),this, SLOT( recv_uirefresh(int)));
}
//将接收到的数据类型发送（eg.短信，联系人）
void daemon::recv_uirefresh(int cmd)
{
        post_refresh(cmd);
}
//发送事件给mainwindow
void daemon::post_refresh(int cmd)
{
    ASSEvent* event_refresh= new ASSEvent(ASSEVENT_REFRESH,cmd);
    QApplication::postEvent(MainWidget,event_refresh);
}

