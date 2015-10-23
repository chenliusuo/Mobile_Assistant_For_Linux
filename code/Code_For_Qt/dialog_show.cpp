#include "dialog_show.h"
#include "ui_dialog_show.h"
#include "thread_1.h"

Dialog_show::Dialog_show(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_show)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(320,100,700,550);
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    QPixmap pixmap3("/tmp/ass/screenshot.png");
    QPixmap pixmap(pixmap3.scaled(310,522, Qt::KeepAspectRatio));
    ui->label->setPixmap(pixmap);
    ui->label->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    //启动线程设置背景为透明
    thread=new thread_1;//新建线程
    connect(thread,SIGNAL(picsignal(int)),this,SLOT(picdisplay(int)));
    QPixmap pixmap1("/tmp/ass/screenshot.png");
    QPixmap pixmap_1(pixmap1.scaled(310,522, Qt::KeepAspectRatio));
    ui->label->setPixmap(pixmap_1);
    ui->label->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    thread->start();//启动线程
    QPixmap pixmap2("/tmp/ass/screenshot.png");
    QPixmap pixmap_2(pixmap2.scaled(310,522, Qt::KeepAspectRatio));
    ui->label->setPixmap(pixmap_2);
    ui->label->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    //ui->startButton->setEnabled(false);
    ui->stopButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/close.png);background-image : url(:/img/pics/nobg.png)}"
                                                    "QToolButton:hover{border-image: url(:/img/pics/p_close.png);}"
                                                    "QToolButton:pressed{border-image: url(:/img/pics/p_close.png);}");
    ui->stopButton->setEnabled(true);
}

void Dialog_show::picdisplay(int a)//设置控件内图片
{
    if(a >=0 )
    {
        QPixmap pixmap1("/tmp/ass/screenshot.png");
        QPixmap pixmap_1(pixmap1.scaled(310,522, Qt::KeepAspectRatio));
        ui->label->setPixmap(pixmap_1);
    }

}

Dialog_show::~Dialog_show()
{
    delete ui;
}


void  Dialog_show::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}

void  Dialog_show::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    last = e->globalPos();
    move(x() + dx, y() + dy);
}

void Dialog_show::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() -last.x();
    int dy = e->globalY() - last.y();

    move(x() + dx,y() + dy);
}




void Dialog_show::on_stopButton_clicked()//停止按钮
{
    //终止线程
    if(thread->isRunning()){
        thread->stop();

        ui->stopButton->setEnabled(false);
    }
    this->close();
}

