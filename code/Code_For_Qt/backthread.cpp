#include "backthread.h"
#include"assevent.h"
#include<QDebug>
extern "C"
{
   #include "recv_contacts.h"
   #include "connect_judg.h"
    #include"package_info.h"
}
ASSEvent::ASSEvent(QEvent::Type etype,int re):
    QEvent(etype),ret(re)
{
}
ASSEvent::~ASSEvent()
{
}

int ASSEvent::msg() const
{
    return ret;
}
BackThread::BackThread(QWidget* objwidget) :
    MainWidget((QObject *)objwidget)
{
}

BackThread::~BackThread()
{
}

void BackThread::recv_refresh(int cmd)
{
    if(cmd != CMD_SEND_MESSAGE)
        qDebug()<<"will send the cmd to the andriod"<<cmd;
    if(cmd != 0)
            send_info(cmd);
}

void BackThread::customEvent(QEvent* e)
{
    ASSEvent* jpe =  dynamic_cast<ASSEvent*>(e);
    switch(jpe->type())
    {
    case ASSEVENT_REFRESH:
            recv_refresh(jpe->msg());
        break;
    default:
        break;
    }
    return;
}
