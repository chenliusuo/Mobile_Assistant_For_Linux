#include "disconnect_thread.h"

Disconnect_thread::Disconnect_thread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
}
Disconnect_thread::~Disconnect_thread()
{

}

void Disconnect_thread::run()
{
     while(!stopped)
    {
        FILE *fp;
        char buf[100];
        fp=popen("/opt/ass/bin/adb devices","r");
        int i=0;
        while(!feof(fp))
         {
            memset(buf,'\0',100);
            if(fgets(buf,100,fp)==NULL)
           {
                 if(i==2)
                 {
                    emit msgSignal(i);//send  sig
                     break;
                    //QProcess::execute("/opt/ass/bin/adb wait-for-device");
                 }
           }
             i++;
        }
             pclose(fp);
            QProcess::execute("sleep 2");
    }
}


void Disconnect_thread::stop()
{
    stopped = true;
}
