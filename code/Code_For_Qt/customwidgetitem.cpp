#include "customwidgetitem.h"
#include <QListWidgetItem>
#include <QLabel>
#include <QGridLayout>
#include <QSqlQuery>
#include <QDebug>
#include <stdio.h>
#include <QToolButton>
#include <QCheckBox>

CustomWidgetItem::CustomWidgetItem(int index,QListWidgetItem *item,QWidget *parent) :
    QWidget(parent),
    index(index),
    currItem(item)
{
      QSqlDatabase db;
      if(QSqlDatabase::contains("qt_sql_default_connection"))
          db = QSqlDatabase::database("qt_sql_default_connection");
      else
          db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName("\/tmp\/ass\/contacts.db3");
      bool success = db.open();

      QSqlQuery query2(db);
      QString sqlStr2="select personName,phoneNumber from contactdb limit ";
      char tmp[20];
      char tmp2[]=",1";
      sprintf(tmp,"%d",index);
      sqlStr2 +=tmp;
      sqlStr2 +=tmp2;

      query2.prepare(sqlStr2);
      if(query2.exec())
      {
          while(query2.next())
          {
              customTitle = QString((query2.value(0).toString()));
              customText=QString((query2.value(1).toString()));
              //qDebug()<<customTitle;
              QLabel *labelTitle = new QLabel(this);
              labelTitle->setStyleSheet("font: 14pt \"黑体\";");
              QLabel *labelContent = new QLabel(this);
              labelContent->setStyleSheet("font: 14pt \"黑体\";");
             // QCheckBox *check=new QCheckBox(this);

              QToolButton *toolButtton = new QToolButton(this);
              toolButtton->setIcon(QIcon("/opt/ass/pics/people.png"));
              toolButtton->setIconSize(QSize(48, 48));
              toolButtton->setAutoRaise(true);
              labelTitle->setText(customTitle);
              labelContent->setText(customText);
             // check->setCheckState(Qt::Unchecked);
              //check->resize();
               QGridLayout *gridLayout = new QGridLayout;
               gridLayout->addWidget(toolButtton,0,1,2,1);
               gridLayout->addWidget(labelTitle,0,2,1,1);
               gridLayout->addWidget(labelContent,1,2,1,1);
               gridLayout->setColumnStretch(0,1);
               gridLayout->setColumnStretch(1,4);
               gridLayout->setColumnStretch(2,6);
               this->setLayout(gridLayout);
               this->resize(this->width(), 60);
                //gridLayout->setContentsMargins(50,50,50,50);//设置布局内部四边的空隙
             this->setObjectName(QString("customWidgetItem%1").arg(index));
               connect(toolButtton, SIGNAL(clicked()), this, SLOT(slotMeClicked()));
          }
      }

}

void CustomWidgetItem::enterEvent(QEvent *)
{
   // this->resize(this->width(), 100);
  //  emit signalMeClicked(customTitle,customText,index);
   // currItem->setSizeHint(this->size());
}
void  CustomWidgetItem::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
     emit signalMeClicked(customTitle,customText,index);
}

void CustomWidgetItem::leaveEvent(QEvent *)
{
  //  this->resize(this->width(), 60);
 //   currItem->setSizeHint(this->size());
}

void CustomWidgetItem::slotMeClicked()
{
    emit signalMeClicked(customTitle,customText,index);
}
