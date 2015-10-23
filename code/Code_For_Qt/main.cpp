#include "mainwindow.h"
#include "ToolButton.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QPixmap>
#include <QElapsedTimer>
#include <QMovie>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMovie *mov = new QMovie(":/img/pics/start.gif");
    QLabel *label = new QLabel("",0);
    label->setGeometry(450,200,500,300);
    label->setMovie(mov);
    mov->start();
    label->setWindowFlags(Qt::FramelessWindowHint);
    label->show();

    QTime t;
    t.start();
    while(t.elapsed() < 1500)
    {
        a.processEvents();
    }


    MainWindow w;
    w.show();
    w.move((QApplication::desktop()->width()-w.width())/2,(QApplication::desktop()->height()-w.height())/2);
    label->close();

    return a.exec();
}
