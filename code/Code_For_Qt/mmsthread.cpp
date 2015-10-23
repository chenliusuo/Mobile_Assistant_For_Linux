#include "mmsthread.h"
#include<QDebug>
Mmsthread::Mmsthread(QObject *parent) :
    QThread(parent)
{
    stopped =false;
}

Mmsthread::~Mmsthread()
{
}

void Mmsthread::run()
{
      while(!stopped)
         {
            sleep(1);
            int send_result = send_info(CMD_RECV_MESSAGE);
            qDebug()<<"send_result is "<<send_result;
          }
        stopped = false;
     qDebug()<<"mmsthread run is over";

}
void Mmsthread::stop()
{
    qDebug()<<"mmsthread stop is running";
    stopped = true;
}
