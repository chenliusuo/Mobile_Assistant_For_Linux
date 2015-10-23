#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include<QProcess>
#include"recvthread.h"
#include"connectionthread.h"
#include"mmsthread.h"

extern "C"
{
   #include"package_info.h"
}
class daemon : public QObject
{
    Q_OBJECT
    public:
        daemon(QWidget*);
        ~daemon();
        void recv_threadinit();

    private:
   
        void post_refresh(int cmd);
        void post_daemon(int cmd);

        void recv_daemoninit();


        void Connecttestinit();
        int checkapp();
    private:
        //MainThread object
        //	ManageWindow* MainWidget;
        QObject* MainWidget;

        Connectthread  *connecttest;
        recv_thread *RecvThread;
        Mmsthread* mmstype;

        QString serialNumber;
        unsigned int timercount;
        private slots:
            void slotConnectionChanged(int,QString);
            void recv_uirefresh(int);

            void mms_over();

    protected:
        void customEvent(QEvent*);
signals:
        void connect_finished();


};

#endif // DAEMON_H
