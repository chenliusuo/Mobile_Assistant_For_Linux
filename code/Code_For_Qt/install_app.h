#ifndef INSTALL_APP_H
#define INSTALL_APP_H

#include <QThread>
#include<QProcess>
#include<QMessageBox>
#include<QDebug>

class Install_app : public QThread
{
    Q_OBJECT
public:
    explicit Install_app(QString appfilename,QObject *parent = 0);
    ~Install_app();
    QString filename;

    void run();
signals:
    void install_finished(QString);
private:
    QString web_app;
public slots:

};

#endif // INSTALL_APP_H
