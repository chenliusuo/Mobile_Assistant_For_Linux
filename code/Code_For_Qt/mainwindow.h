#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QToolButton>
#include<QSqlDatabase>
#include<QTableWidget>
#include<QTableWidgetItem>
#include<QPushButton>
#include<QEvent>
#include<QtWebKitWidgets/QWebPage>
#include<QFileDialog>
#include"webpage.h"
#include"assevent.h"
#include"backthread.h"
#include"daemon.h"
#include"mms_dialog.h"
#include "thread_2.h"
#include "dialog.h"
#include"install_app.h"
#include"disconnect_thread.h"
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include<QCheckBox>
#include <QTreeWidgetItem>
#include<QGridLayout>

extern "C"
{
   #include"package_info.h"
}

namespace Ui {
class MainWindow;
}
class QSqlTableModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
       void startRequest(QUrl url);
       message_pack_t uimmsinfo;
       message_t sendmms;
       message_t recvmms;
       contact_cmd_t  change_contact;
       audio_pack_t uiaudioinfo;
       image_pack_t uiimageinfo;
       video_pack_t uivideoinfo;

private:
    QSqlDatabase db1;

    QTableWidget* Mms_table;
    QTableWidgetItem *Mms_item[2000][6];

    //read the third app
    QTableWidgetItem *app_table_widget_item[100][6];

    QTableWidget* App_table;
    QTableWidgetItem *App_item[300][6];

     Install_app *local_appinstall;

   QListWidget *listWidget_File;
   QListWidget *list_widgetvideo;

    QTableWidget* Audio_table;
    QTableWidgetItem* Audio_item[500][5];

    QTableWidget* Video_table;
    QTableWidgetItem*  Video_item[300][5];

    MMS_Dialog *mms_dialog;
    MMS_Dialog *contact_mmsdialog;
    char MMSSIGN = -1;
    char BOOKSIGN = -1;
    char APPTYPE = 0;
    char AUDIOXSIGN = -1;
    QLabel* label;
    void showPicture(const QModelIndex &index);

    QGridLayout *contactview_grid;

    int imagewidth;
    int imageheight;

     QLabel *showload_audio;
     QLabel *showload_image;
     QLabel *showload_mms;
     QLabel *showload_contacts;
     QLabel *showload_video;
     QLabel *showload_app;

    QString formattime(int );
    QString formatsize(int);

       //联系人数据
    int  contact_db_index;
    QString contact_db_title;
    QString contact_db_text;

    //path
    char chosed_path[1024];
    char send_path[1024];
    char get_file_path[1024];

    int send_file_exec_count;
private slots:
    void pushButton_9_choose();

    void on_getlist_clicked();

    void on_toolButton_4_clicked();

    void on_toolButton_5_clicked();

    void on_toolButton_6_clicked();

    void on_toolButton_7_clicked();

    void on_toolButton_8_clicked();

    void on_toolButton_9_clicked();

    //void on_pushButton_6_clicked();//联系人delete
    void pushButton_6_del();

    void on_pushButton_7_clicked();

    void on_show_button_1_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    //getfile
    void get_file_slot();

    void on_webView_linkClicked(const QUrl &arg1);

    void httpFinished();

   void httpReadyRead();
   void updateDataReadProgress(qint64,qint64);

   void Mms_clicked(int, int);
   void goto_mmsdetail();
   void goto_mmsrefresh();
   void goto_sendmms();
   void get_mmsinfo();
   void goto_audio();
   void goto_checkaudio(int,int);
   void goto_audiobackup();

   void goto_appinstall();
   void slotappchange(int);
   void goto_appbackup();
   void goto_appuninstall();
   void goto_check(int ,int);
   void goto_image();
   void goto_video();
   void imagestateChanged(int);
   void videostateChanged(int);
   void goto_imagebackup();
   void goto_videobackup();

   void setui_contacts();
   void setui_mmsinfo();
   void setui_app(int);
   void setui_apps();
   void setui_systemapp();
   void setui_image();
   void setui_audio();
   void setui_video();
   void setui_recvmms();

   void contact_sendmms();
   void get_contactmms();

    void display(int);

    void Audio_clicked(int,int);
    void on_Btn_mmsrefresh_clicked();

    void on_toolButton_10_clicked();

    void on_toolButton_clicked();

    void on_toolButton_12_clicked();

    void on_pushButton_4_clicked();



    void on_toolButton_14_clicked();

     void showSelectedList(QTreeWidgetItem * item); //点击树节点事件
    void showClickedList(QTreeWidgetItem *item);//on click
    void choose_dir(QTreeWidgetItem *item);//on click
    //通讯录

    void slotComboboxActivated(int index);
    void slotItemClicked(QString customTitle,QString customText,int index);

    void on_Btn_audio_clicked();

    void on_Btn_image_clicked();

    void on_Btn_video_clicked();

    //show down progress slot
    void adddownloadurl(int,QString,int,int,QString);

        void connect_finished();
        void appinstall_finished(QString);
        void on_toolButton_11_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *contact_model;
 //   QSqlTableModel *modelapp;
    //QSqlTableModel *modelsysapp;
    QPoint last;
    BackThread *Backthread;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QUrl url;
/*********/
    QStringList list;
    QStringList appunlist;
    QStringList appback;
    QStringList musiclist;
    QStringList imagelist;
    QStringList videolist;

    QStringList Appline_count;
    QFile *file;
    QLineEdit *urladdr;
    QPushButton *goBack;
    QPushButton *goForward;
    QPushButton *pageReload;
    QPushButton *pageStop;
    void LoadWebview();
    QWebPage *page;
    daemon* DaemonThread;
    Dialog *showdialog;

    Disconnect_thread *Disconnect_Thread;    //连接监控槽

    //联系人显示
    QListWidget *comboboxListWidget;

    //QLabel
    QLabel *tx1;
    QLabel *tx2;
    QLabel *tx3;
    QLabel *tx4;
    QLabel *tx5;

    QLabel *memory;
     QLabel *sd;
     QLabel *bat;
//battery bar
    QProgressBar *bar1;
    QProgressBar *bar2;

    void screenshotinit();
    void connect_init();
    void UiToolInit();

    void LoadMms();
    void LoadApp();
    void LoadMusicWebview();
    void LoadImage();
    void LoadAudio();
    void LoadVideo();


    void post_refresh(int);
    void post_daemon();
    void customEvent(QEvent* e);

    void DaemonThreadinit();
    void BackThreadinit();

    void recv_refresh(int);
    //file manage
    void sys_init();
    void inside_init();
    void outside_init();

    void setui_clean();

//add to show download progress
    void gridlayout_init();

    void Clear_all();
signals:
    void device_offline();

};


#endif // MAINWINDOW_H
