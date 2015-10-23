#ifndef CUSTOMWIDGETITEM_H
#define CUSTOMWIDGETITEM_H

#include <QWidget>
#include <QGridLayout>
#include<QMouseEvent>
class QListWidgetItem;

class CustomWidgetItem : public QWidget
{
    Q_OBJECT
public:
    explicit CustomWidgetItem(int index,QListWidgetItem *item,QWidget *parent=0);
    int index;
    QString customTitle;
    QString customText;
    QObject* MainWidget;
signals:
    void signalMeClicked(QString customTitle,QString customText,int index);


protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    QListWidgetItem *currItem;
    void mousePressEvent(QMouseEvent *e);

    QPoint last;
private slots:
    void slotMeClicked();
   // void signalCheck(int );//checkbox

};

#endif // CUSTOMWIDGETITEM_H
