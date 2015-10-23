#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include<QObject>
#include<QApplication>
#include<QMainWindow>
#include<QThread>

class recv_thread:public QThread
{
    Q_OBJECT
public:
    //recv_thread(QWidget*);
    ~recv_thread();
      void  run();

private:

   QObject* MainWidget;
signals:
   void recv_signals(int);
};
#endif // RECVTHREAD_H
