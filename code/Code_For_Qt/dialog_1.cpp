#include "dialog_1.h"
#include "ui_dialog_1.h"
#include "thread_1.h"

Dialog_1::Dialog_1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_1)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(250,50,500,700);
    ui->label->setGeometry(60,30,387.5,652.5);
    QPixmap pixmap1("/tmp/ass/screenshot.png");
    QPixmap pixmap(pixmap1.scaled(387.5,652.5, Qt::KeepAspectRatio));
    ui->label->setPixmap(pixmap);
    ui->label->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
}

Dialog_1::~Dialog_1()
{
    delete ui;
}
