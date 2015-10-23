#ifndef ASSEVENT_H
#define ASSEVENT_H

#include<QEvent>
#include<QApplication>
#include<QObject>

const QEvent::Type ASSEVENT_REFRESH = (QEvent::Type)3001;
const QEvent::Type ASSEVENT_DAEMON = (QEvent::Type)3002;

class ASSEvent:public QEvent
{
public:
    ASSEvent(QEvent::Type, int);
    ~ASSEvent();

    int msg() const;

private:
    int ret;
};

#endif // ASSEVENT_H
