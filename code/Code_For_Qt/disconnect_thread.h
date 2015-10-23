#ifndef DISCONNECT_THREAD_H
#define DISCONNECT_THREAD_H

#include <QThread>
#include<QProcess>
#include <QDebug>
extern "C"
{
#include<stdio.h>
#include<string.h>
}
class Disconnect_thread : public QThread
{
    Q_OBJECT
public:
    explicit Disconnect_thread(QObject *parent = 0);
    ~Disconnect_thread();
    void stop();
signals:
 void msgSignal(int);//信息传出信号
public slots:
protected:
   void run();

private:
     volatile bool stopped;
};

#endif // DISCONNECT_THREAD_H
