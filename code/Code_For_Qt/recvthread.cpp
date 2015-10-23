#include"recvthread.h"
#include<QWidget>
#include<QDebug>
extern "C"
{
   #include"package_info.h"
}

recv_thread::~recv_thread()
{
}

void recv_thread::run()
{
   //qreal i=0;
   int cmdinfo = 0;

   while(1)
   {
        cmdinfo = recv_info();
        msleep(10);

       if(cmdinfo )
       {
           if(cmdinfo != CMD_RECV_MESSAGE)
                     qDebug()<<"recvthread have recv something"<<cmdinfo;
          emit recv_signals(cmdinfo);
        }
  }

}
