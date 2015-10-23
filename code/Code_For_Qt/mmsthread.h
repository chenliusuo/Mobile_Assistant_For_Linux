#ifndef MMSTHREAD_H
#define MMSTHREAD_H

#include <QThread>
extern "C"
{
   #include"package_info.h"
}
class Mmsthread : public QThread
{
    Q_OBJECT
public:
    explicit Mmsthread(QObject *parent = 0);
    ~Mmsthread();

    void run();
    void stop();

private:
    volatile bool stopped;

signals:

public slots:

};

#endif // MMSTHREAD_H
