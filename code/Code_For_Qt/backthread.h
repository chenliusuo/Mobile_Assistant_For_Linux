#ifndef BACKTHREAD_H
#define BACKTHREAD_H

#include <QObject>
#include "assevent.h"

class BackThread : public QObject
{
    Q_OBJECT
public:
    explicit BackThread(QWidget*);
    ~BackThread();
private:
    void recv_refresh(int);

    QObject* MainWidget;

protected:
    void customEvent(QEvent *);

signals:
    void recv_finished();

public slots:

};

#endif // BACKTHREAD_H
