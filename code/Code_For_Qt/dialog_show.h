#ifndef DIALOG_SHOW_H
#define DIALOG_SHOW_H

#include <QDialog>
#include"thread_1.h"
#include <QMouseEvent>
namespace Ui {
class Dialog_show;
}

class Dialog_show : public QDialog
{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

public:
    explicit Dialog_show(QWidget *parent = 0);
    ~Dialog_show();

private slots:

    void on_stopButton_clicked();
    //pic
    void picdisplay(int);//接收线程信号

private:
    Ui::Dialog_show *ui;
    thread_1 *thread;//定义线程
    QPoint last;
};

#endif // DIALOG_SHOW_H
