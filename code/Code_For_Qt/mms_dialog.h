#ifndef MMS_DIALOG_H
#define MMS_DIALOG_H

#include <QDialog>

namespace Ui{
    class MMS_Dialog;
}
class MMS_Dialog : public QDialog
{
    Q_OBJECT

protected:
    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

public:
    explicit MMS_Dialog(QWidget *parent = 0);
    ~MMS_Dialog();
    Ui::MMS_Dialog *ui;

    void sendnumber(QString);

    QString mmsnumber;
    QString mmsdata;
    QPoint last;

  private:
    void get_data();

signals:
    void mms_accept();

public slots:
   void dialog_accept();
private slots:
   void on_pushButton_clicked();
};

#endif // MMS_DIALOG_H
