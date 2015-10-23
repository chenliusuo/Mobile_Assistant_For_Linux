#ifndef THREAD_2_H
#define THREAD_2_H

#include <QThread>

class thread_2 : public QThread
{
    Q_OBJECT
public:
    explicit thread_2(QObject *parent = 0);

signals:

 void connect_find(int);
public slots:
protected:
    void run();
};

#endif // THREAD_2_H
