#include "install_app.h"

Install_app::Install_app(QString filename,QObject *parent) :
    filename(filename),
    QThread(parent)
{
    web_app = filename;
}
Install_app::~Install_app()
{
}

void Install_app::run()
{
    QProcess *process1=new QProcess;
    QString output_msg;
    output_msg = "";

    qDebug()<<"install app thread is running";
    qDebug()<<web_app;

    process1->start(web_app);
    process1->waitForFinished();
    QThread::sleep(2);
    output_msg = process1->readAll();
    qDebug()<<output_msg;

    if(output_msg.contains("Failure"))
    {
        emit install_finished("Failure");
        output_msg = "";
    }
    else
    {
        emit install_finished("Success");
        output_msg = "";
    }

}
