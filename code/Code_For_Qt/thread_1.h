#ifndef THREAD_1_H
#define THREAD_1_H

#include <QThread>

class thread_1 : public QThread
{
    Q_OBJECT
public:
    explicit thread_1(QObject *parent = 0);
    void stop();
    //void run();
signals:
   void picsignal(int);

public slots:
    //void sig;
protected:
    void run();
private:
    volatile bool stopped;
   // void run();

};

#endif // THREAD_1_H
