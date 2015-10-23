#include "ui_MMS_dialog.h"
#include"mms_dialog.h"
#include <QBitmap>
#include <QPainter>

MMS_Dialog::MMS_Dialog(QWidget *parent) :
    QDialog(parent),
  ui(new Ui::MMS_Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Send Message");
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(300,130,481,435);
    get_data();
    ui->MMS_send->setStyleSheet("QPushButton{border-image: url(:/img/pics/send.png);background-image : url(:/img/pics/nobg.png)}"
                                "QPushButton:hover{border-image: url(:/img/pics/h_send.png);}"
                                "QPushButton:pressed{border-image: url(:/img/pics/p_send.png);}");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(:/img/pics/c.png);background-image : url(:/img/pics/nobg.png)}"
                                "QPushButton:hover{border-image: url(:/img/pics/h_c.png);}"
                                "QPushButton:pressed{border-image: url(:/img/pics/h_ct.png);}");
}
MMS_Dialog::~MMS_Dialog()
{
    delete ui;
}

void MMS_Dialog::get_data()
{
    connect(ui->MMS_send,SIGNAL(clicked()), this, SLOT(dialog_accept()));
}

void MMS_Dialog::dialog_accept()
{
    mmsnumber = ui->mmstext_number->toPlainText();
    mmsdata = ui->mmstext_data->toPlainText();

    emit mms_accept();
    this->accept();
}

void MMS_Dialog::sendnumber(QString number)
{
    ui->mmstext_number->setText(number);
}
void MMS_Dialog::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}

void MMS_Dialog::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    last = e->globalPos();
    move(x() + dx, y() + dy);
}

void MMS_Dialog::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() -last.x();
    int dy = e->globalY() - last.y();

    move(x() + dx,y() + dy);
}

void MMS_Dialog::on_pushButton_clicked()
{
    this->close();
}
