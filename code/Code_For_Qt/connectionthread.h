#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include <QThread>
#include<QProcess>
#include"thread_2.h"
class Connectthread : public QThread
{
    Q_OBJECT
public:
    explicit Connectthread(QObject *parent = 0);
    ~Connectthread();
    QObject* MainWidget;
    void run();

private:
    thread_2 *connect_change;
signals:
            void connectionChanged(int,QString);

public slots:
            void connect_success(int);


};

#endif // CONNECTIONTHREAD_H
