#include "dialog.h"
#include "ui_dialog.h"
#include <QMovie>
#include <QDesktopWidget>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QMovie *mov = new QMovie(":/img/pics/1.gif");
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setGeometry(570,270,200,200);
    ui->label->setMovie(mov);
    mov->start();
}

Dialog::~Dialog()
{
    delete ui;
}
