#include "mainwindow.h"
#include "ToolButton.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QStringListModel>
#include <QtSql>
#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QTableView>
#include <QTextBrowser>
#include <QToolButton>
#include<QMessageBox>
#include <QButtonGroup>
#include <QMovie>
#include <QProgressBar>
//#include<QWebPage>
#include "dialog.h"
#include "dialog_show.h"
#include"recvthread.h"
#include <QMutex>
#include "customwidgetitem.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     DaemonThreadinit();//daemon init
     BackThreadinit();//后台程序初始化

    UiToolInit();//界面初始化
  //连接模块建立
  // screenshotinit();

     manager = new QNetworkAccessManager(this);
     LoadWebview();//安卓市场页面加载
   //LoadImage();

     showload_image = new QLabel(ui->tab_8);
     showload_audio = new QLabel(ui->tab_7);
     showload_video = new QLabel(ui->tab_4);
     showload_contacts = new QLabel(ui->tab_2);
     showload_app = new QLabel(ui->tab_5);
     showload_mms = new QLabel(ui->tab_3);


    connect(ui->Btn_image,SIGNAL(clicked()),this,SLOT(goto_image()));//相册导入
    connect(ui->Btn_video,SIGNAL(clicked()),this,SLOT(goto_video()));//视频导入
    ui->show_button_1->setEnabled(false);
     ui->toolButton_2->setEnabled(false);
     ui->toolButton_3->setEnabled(false);
     ui->getlist->setEnabled(false);
     ui->pushButton->setEnabled(false);
     ui->pushButton_3->setEnabled(false);
     ui->pushButton_6->setEnabled(false);
     ui->Btn_mmsrefresh->setEnabled(false);
     ui->Btn_sendmms->setEnabled(false);
     ui->Btn_audio->setEnabled(false);
     ui->Btn_audioBackup->setEnabled(false);
     ui->Btn_image->setEnabled(false);
     ui->Btn_imageBackup->setEnabled(false);
     ui->Btn_video->setEnabled(false);
   //  ui->Btn_videBackup->setEnabled(false);
     ui->pushButton_2->setEnabled(false);
      ui->pushButton_9->setEnabled(false);
      ui->Btn_appbackup->setEnabled(false);
      ui->Btn_appinstlal->setEnabled(false);
      ui->Btn_uninstall->setEnabled(false);
      ui->pushButton_4->setEnabled(false);
      ui->Btn_back->setEnabled(false);
      ui->Btn_forward->setEnabled(false);
      ui->Btn_reload->setEnabled(false);
      //ui->pushButton_5->setEnabled(false);

}

MainWindow::~MainWindow()
{
    set_end_flag(2);
    Clear_all();
    delete ui;
}

//界面的初始化
void MainWindow::UiToolInit()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    //this->setGeometry(270,150,880,600);

    ui->show_button_1->setStyleSheet("QToolButton{border-image: url(:/img/pics/jp.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/p_jp.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_jp.png);}");
    ui->toolButton_2->setStyleSheet("QToolButton{border-image: url(:/img/pics/ys.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/p_ys.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_ys.png);}");
    ui->toolButton_3->setStyleSheet("QToolButton{border-image: url(:/img/pics/sx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/p_sx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sx.png);}");
    ui->getlist->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    //ui->getlist->setToolTip("开始导入");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(:/img/pics/xg.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_xg.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_xg.png);}");
    //ui->pushButton->setToolTip("保存修改");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(:/img/pics/add.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_add.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_add.png);}");
    //ui->pushButton_3->setToolTip("添加联系人");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(:/img/pics/delete.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_delete.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_delete.png);}");
    //ui->pushButton_6->setToolTip("删除联系人");
    ui->Btn_back->setStyleSheet("QPushButton{border-image: url(:/img/pics/back.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/p_back.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/p_back.png);}");
    ui->Btn_reload->setStyleSheet("QPushButton{border-image: url(:/img/pics/reload.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/p_reload.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/p_reload.png);}");
    ui->Btn_forward->setStyleSheet("QPushButton{border-image: url(:/img/pics/go.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/p_go.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/p_go.png);}");
    ui->Btn_mmsrefresh->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    //ui->Btn_mmsrefresh->setToolTip("开始导入");
    ui->Btn_sendmms->setStyleSheet("QPushButton{border-image: url(:/img/pics/hf.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_hf.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_hf.png);}");
    //ui->Btn_sendmms->setToolTip("回复信息");
    ui->Btn_appinstlal->setStyleSheet("QPushButton{border-image: url(:/img/pics/local.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_local.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_local.png);}");
    ui->Btn_appbackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_audio->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                 "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                 "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_image->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                 "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                 "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_uninstall->setStyleSheet("QPushButton{border-image: url(:/img/pics/delete.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_delete.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_delete.png);}");
    ui->Btn_video->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                 "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                 "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_audioBackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_imageBackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_videBackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");

    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_home.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_10->setStyleSheet("QToolButton{border-image: url(:/img/pics/c.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_c.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/h_c.png);}");
    ui->toolButton_11->setStyleSheet("QToolButton{border-image: url(:/img/pics/l.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_l.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/h_l.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
    //                                "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
       //                             "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(:/img/pics/connect.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_connect.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_connect.png);}");
    ui->show_button_1->setStyleSheet("QToolButton{border-image: url(:/img/pics/jp.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/p_jp.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_jp.png);}");
    ui->toolButton_2->setStyleSheet("QToolButton{border-image: url(:/img/pics/ys.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/p_ys.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_ys.png);}");
    ui->toolButton_3->setStyleSheet("QToolButton{border-image: url(:/img/pics/sx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/p_sx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sx.png);}");
    ui->getlist->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(:/img/pics/add.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_add.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_add.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(:/img/pics/delete.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_delete.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_delete.png);}");
    ui->Btn_back->setStyleSheet("QPushButton{border-image: url(:/img/pics/back.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/p_back.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/p_back.png);}");
    ui->Btn_reload->setStyleSheet("QPushButton{border-image: url(:/img/pics/reload.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/p_reload.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/p_reload.png);}");
    ui->Btn_forward->setStyleSheet("QPushButton{border-image: url(:/img/pics/go.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/p_go.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/p_go.png);}");
//    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(:/img/pics/serchfor.png);background-image : url(:/img/pics/nobg.png)}"
//                                    "QPushButton:hover{border-image: url(:/img/pics/p_serchfor.png);}"
//                                    "QPushButton:pressed{border-image: url(:/img/pics/p_serchfor.png);}");
    ui->Btn_mmsrefresh->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_sendmms->setStyleSheet("QPushButton{border-image: url(:/img/pics/hf.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_hf.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_hf.png);}");
    ui->Btn_appinstlal->setStyleSheet("QPushButton{border-image: url(:/img/pics/local.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_local.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_local.png);}");
    ui->Btn_appbackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_audio->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                 "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                 "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_image->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                 "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                 "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_uninstall->setStyleSheet("QPushButton{border-image: url(:/img/pics/delete.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_delete.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_delete.png);}");
    ui->Btn_video->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                 "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                 "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->Btn_audioBackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_imageBackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_videBackup->setStyleSheet("QPushButton{border-image: url(:/img/pics/bf.png);background-image : url(:/img/pics/nobg.png)}"
                                     "QPushButton:hover{border-image: url(:/img/pics/h_bf.png);}"
                                     "QPushButton:pressed{border-image: url(:/img/pics/h_bf.png);}");
    ui->Btn_contactsendmms->setStyleSheet("QPushButton{border-image: url(:/img/pics/hf.png);background-image : url(:/img/pics/nobg.png)}"
                                          "QPushButton:hover{border-image: url(:/img/pics/h_hf.png);}"
                                          "QPushButton:pressed{border-image: url(:/img/pics/h_hf.png);}");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(:/img/pics/xg.png);background-image : url(:/img/pics/nobg.png)}"
                                  "QPushButton:hover{border-image: url(:/img/pics/h_xg.png);}"
                                  "QPushButton:pressed{border-image: url(:/img/pics/h_xg.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(:/img/pics/dr2.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_dr2.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_dr2.png);}");
    ui->pushButton_9->setStyleSheet("QPushButton{border-image: url(:/img/pics/dc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QPushButton:hover{border-image: url(:/img/pics/h_dc.png);}"
                                    "QPushButton:pressed{border-image: url(:/img/pics/h_dc.png);}");

   tx1 =new QLabel(ui->tab);
   tx2 =new QLabel(ui->tab);
   tx3 = new QLabel(ui->tab);
   bar1 =new QProgressBar(ui->tab);
   tx4 = new QLabel(ui->tab);
   bar2 =new QProgressBar(ui->tab);
   tx5 = new QLabel(ui->tab);
   bat = new QLabel(ui->tab);
  memory = new QLabel(ui->tab);
   sd = new QLabel(ui->tab);
   tx1->hide();
   tx2->hide();
   tx3->hide();
   tx4->hide();
   tx5->hide();
   bar1->hide();
   bar2->hide();
  memory->setStyleSheet("background-image:url(:/img/pics/nobg.png)");
  sd->setStyleSheet("background-image:url(:/img/pics/nobg.png)");
  bat->setStyleSheet("background-image:url(:/img/pics/nobg.png)");


     LoadMms();
    LoadApp();
    LoadAudio();
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(pushButton_6_del()));//联系人删除按钮绑定
    connect(ui->Btn_contactsendmms,SIGNAL(clicked()),this,SLOT(contact_sendmms()));

    ui->label_2->hide();
    ui->label_5->hide();
    ui->label_4->hide();
    ui->lineEdit->hide();
    ui->lineEdit_3->hide();

    ui->label_8->hide();
    ui->label_9->hide();
    ui->label_10->hide();
    ui->label_11->hide();
    ui->toolButton_13->hide();


}
//短信界面加载
void MainWindow::LoadMms()
{
   Mms_table = new QTableWidget(ui->mms_widget);
    Mms_table->horizontalHeader()->setStyleSheet("QHeaderView::section{background-image:url(:/img/pics/bg2.png);border-image:url(:/img/pics/bg2.png);color:black;}");
   Mms_table->setGeometry(0,0,781,500);
   Mms_table->setStyleSheet("background-image : url(:/img/pics/bg2.png)");
   Mms_table->setMouseTracking(true);
   Mms_table->setColumnCount(4);
   Mms_table->setColumnWidth(0,150);
   Mms_table->setColumnWidth(1,360);
   Mms_table->setColumnWidth(2,180);
   Mms_table->setColumnWidth(3,70);

   Mms_table->setShowGrid(false);
   //Mms_table->setAlternatingRowColors(true);

   Mms_table->verticalHeader()->setVisible(false);;
   Mms_table->horizontalHeader()->setVisible(true);

   QStringList labels;
   labels<<tr("联系人")<<tr("内容")<<tr("日期")<<tr("发送/接收");
   Mms_table->setHorizontalHeaderLabels(labels);

   connect(Mms_table,SIGNAL(cellClicked(int,int)),this,SLOT(Mms_clicked(int,int)));//检测table中元素点击事件
   connect(Mms_table,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this, SLOT(goto_mmsdetail()));//双击查看短信详细内容
   connect(ui->Btn_mmsrefresh,SIGNAL(clicked()),this, SLOT(goto_mmsrefresh()));//短信刷新按钮,信号与槽的绑定
   connect(ui->Btn_sendmms,SIGNAL(clicked()),this,SLOT(goto_sendmms()));//短信回复
}
//获取点击短信位置
void MainWindow::Mms_clicked(int a, int b)
{
    MMSSIGN = a;
}
//对话框的形式展示短信详情
void MainWindow::goto_mmsdetail()
{
    if(MMSSIGN >= 0 )
    {
        QMessageBox::about(this,tr("Mms detail"),QString(uimmsinfo.message_item[MMSSIGN].message_name)+
                           "<p>" + QString(uimmsinfo.message_item[MMSSIGN].message_body) + "<p>" + QString(uimmsinfo.message_item[MMSSIGN].message_date));
    }

}
//发送指令获取短信
void MainWindow::goto_mmsrefresh()
{
    post_refresh(CMD_MESSAGE);
    Mms_table->show();
    Mms_table->setEnabled(false);
    qDebug()<<"send mms cmd to andriod";
    QMovie *mov = new QMovie(":/img/pics/1.gif");
    showload_mms->setMovie(mov);
     showload_mms->setGeometry(360,160,300,300);
     showload_mms->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
     showload_mms->show();
     mov->start();
}
void MainWindow::goto_sendmms()
{
    if( MMSSIGN >= 0)
    {
        mms_dialog = new MMS_Dialog;
        mms_dialog->show();

        mms_dialog->sendnumber(QString(uimmsinfo.message_item[MMSSIGN].message_num));
        connect(this->mms_dialog,SIGNAL(mms_accept()),this, SLOT(get_mmsinfo()));
    }
}
//获取短信号码以及信息，将其拷贝到结构体，并发送给手机端操作
void MainWindow::get_mmsinfo()
{
    memset(&sendmms,'\0',sizeof(message_t));
    if((mms_dialog->mmsnumber != "") && (mms_dialog->mmsdata !="" ))
    {
        QByteArray number_list = mms_dialog->mmsnumber.toLatin1();
        char *char_mmsnumber = number_list.data();

        QByteArray data_list = mms_dialog->mmsdata.toUtf8();
        char *char_mmsdata = data_list.data();
        strcpy(sendmms.message_num,char_mmsnumber);
        strcpy(sendmms.message_body,char_mmsdata);

       set_message_cmd(&sendmms);//将号码和内容拷贝进将要发送的结构体
       post_refresh(CMD_SEND_MESSAGE);//给手机端发送发送短信的指令
    }

}
//将接收到的短信显示到界面上去
void MainWindow::setui_mmsinfo()
{
   // LoadMms();
    qDebug()<<"setui_mmsinfo is runing";
    memset(&uimmsinfo,'\0',sizeof(message_pack_t));
    get_message_pack(&uimmsinfo);

    Mms_table ->setEnabled(true);
    Mms_table->setRowCount(0);

    Mms_table->clearContents();

    Mms_table->setRowCount(uimmsinfo.count+1);
    for(int i =0; i< uimmsinfo.count;i++)
    {
        for(int j =0; j < 4; j++)
        {
            Mms_item[i][j] = new QTableWidgetItem();
            if(j != 1)
            {
                Mms_item[i][j]->setTextAlignment(Qt::AlignCenter);
            }
            Mms_table->setItem(i,j,Mms_item[i][j]);
        }
        Mms_item[i][0]->setText(QString(uimmsinfo.message_item[i].message_name));
        Mms_item[i][1]->setText(QString(uimmsinfo.message_item[i].message_body));
        Mms_item[i][2]->setText(QString(uimmsinfo.message_item[i].message_date));
       if(uimmsinfo.message_item[i].message_type == 1)
              Mms_item[i][3]->setIcon(QIcon(QPixmap(":/img/pics/m_rev.png")));
        else
           Mms_item[i][3]->setIcon(QIcon(QPixmap(":/img/pics/m_send.png")));
       if(uimmsinfo.message_item[i].message_read == 0)
            Mms_item[i][0]->setIcon(QIcon(QPixmap(":/img/pics/m_noread.png")));
        else
             Mms_item[i][0]->setIcon(QIcon(QPixmap(":/img/pics/m_read.png")));
    }
    if(showload_mms != NULL)
         showload_mms->close();
}
//收到新的消息之后弹出新的对话框
void MainWindow::setui_recvmms()
{

    memset(&recvmms,0,sizeof(message_t));
    get_message(&recvmms);
    QMessageBox::about(this,tr("你有新消息"),QString(recvmms.message_name)+
                       "<p>" + QString(recvmms.message_body) + "<p>" + QString(recvmms.message_date));

}
//app界面加载
void MainWindow::LoadApp()
{
    ui->cmb_app->addItem("第三方应用");
    ui->cmb_app->addItem("系统应用");
    ui->cmb_app->setMaxVisibleItems(5);
    ui->cmb_app->setMaxCount(5);

    ui->table_widget_third->horizontalHeader()->setStyleSheet("QHeaderView::section{background-image:url(:/img/pics/bg2.png);border-image:url(:/img/pics/bg2.png);color:black;}");
    ui->table_widget_third->setColumnCount(5);
    ui->table_widget_third->setColumnWidth(0,30);
    ui->table_widget_third->setColumnWidth(1,50);
    ui->table_widget_third->setColumnWidth(2,150);
    ui->table_widget_third->setColumnWidth(3,250);
    ui->table_widget_third->setColumnWidth(4,283);


    QStringList labels;
    labels<<tr("")<<tr("")<<tr("名称")<<tr("大小")<<tr("包名");
    ui->table_widget_third->setHorizontalHeaderLabels(labels);
    ui->table_widget_third->verticalHeader()->setVisible(false);//hide number list
    //ui->table_widget_third->setAlternatingRowColors(true);
    ui->table_widget_third->setFrameShape(QFrame::NoFrame);
    ui->table_widget_third->setShowGrid(false);
    ui->table_widget_third->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_widget_third->setMouseTracking(true);

    connect(ui->cmb_app,SIGNAL(currentIndexChanged(int)),this,SLOT(slotappchange(int)));
    connect(ui->Btn_appinstlal,SIGNAL(clicked()),this,SLOT(goto_appinstall()));//本地安装app按钮
    connect(ui->Btn_appbackup,SIGNAL(clicked()),this,SLOT(goto_appbackup()));//导出app到本地
    connect(ui->Btn_uninstall,SIGNAL(clicked()),this,SLOT(goto_appuninstall()));//卸载app
    connect(ui->table_widget_third,SIGNAL(cellClicked(int,int)),this,SLOT(goto_check(int,int)));//复选框

}
//app下拉菜单发生变化的时候发送指令获取信息
void MainWindow::slotappchange(int appclass)
{
    setui_app(appclass);
}
//根据下拉菜单显示系统应用或者第三方应用
void MainWindow::setui_app(int appclass)
{
    qDebug()<<appclass;
    switch(appclass)
    {
        case 0:
            qDebug()<<"第三方应用";
            setui_apps();
            APPTYPE = 0;
            break;
        case 1:
           qDebug()<<"系统应用";
           setui_systemapp();
           APPTYPE  = 1;
        break;
    default:
        break;
    }
}
//将第三方的app显示到界面上
void MainWindow::setui_apps()
{
    ui->table_widget_third->clearContents();
    ui->table_widget_third->setRowCount(0);

    int a;//get the line num

    //if(QSqlDatabase::contains("qt_sql_default_connection"))
      //  db1 = QSqlDatabase::database("qt_sql_default_connection");
   // else
        db1 = QSqlDatabase::addDatabase("QSQLITE");
    db1.setDatabaseName("\/tmp\/ass\/appinfo.db3");
    bool success = db1.open();

     QSqlQuery query1(db1);
      QString sqlStr1="select  count(*) from thirddb";
       query1.prepare(sqlStr1);
   if(query1.exec())
    {
       while(query1.next())
        {
            a=query1.value(0).toInt();
            break;
        }
    }
   ui->table_widget_third->setRowCount(a);
    for(int i=0;i<a;i++)
    {
        QSqlQuery query2(db1);
        QString sqlStr2="select appName,appsize,packageName from thirddb limit ";
        int m=i + 1;

        char tmp[20];
        char tmp2[20];
       sprintf(tmp,"%d",m);
       sprintf(tmp2,"%d",i);
        sqlStr2 += tmp;
        qDebug()<<tmp;
        query2.prepare(sqlStr2);
         if(query2.exec())
         {
                while(query2.next())
                {
                    for(int j=2;j<5;j++)
                    {
                        app_table_widget_item[i][j]=new QTableWidgetItem;
                        ui->table_widget_third->setItem(i,j,app_table_widget_item[i][j]);
                        if(j != 3)
                        {
                                    app_table_widget_item[i][j]->setText(query2.value(j-2).toString());
                         }else
                          if(j == 3)
                           {
                                     int size = query2.value(1).toInt();
                                    QString appstr = formatsize(size);
                                    app_table_widget_item[i][j]->setText(appstr);
                               }
                         app_table_widget_item[i][j]->setTextAlignment(Qt::AlignVCenter);
                    }
                 }
        }
         QTableWidgetItem *checkBox = new QTableWidgetItem();// 加入复选框
         checkBox->setCheckState(Qt::Unchecked);
         ui->table_widget_third->setItem(i,0,checkBox);
         ui->table_widget_third->setRowHeight(i,50);
         QSqlQuery query3(db1);
         QString sqlStr3="select appid from thirddb limit ";
          sqlStr3 = sqlStr3 + tmp2+","+ "1";

          query3.exec(sqlStr3);
          if(query3.first())
          {
           QString appiconpath;
           appiconpath = "/tmp/ass/icon/"+query3.value(0).toString()+".png";
            QLabel *label1 = new QLabel;
            label1->clear();
           label1->setPixmap(QPixmap(appiconpath));
           label1->setScaledContents(true);//将整个图片的内容铺满整个label页面
            ui->table_widget_third->setCellWidget(i,1,label1);
          }
    }
    if(showload_app != NULL)
            showload_app->close();
}
//将系统预装的app显示到界面上去
void MainWindow::setui_systemapp()
{
    ui->table_widget_third->clearContents();//清空页面上显示的内容
    ui->table_widget_third->setRowCount(0);
    int a;
    QSqlDatabase db2;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db2 = QSqlDatabase::database("qt_sql_default_connection");
    else
        db2 = QSqlDatabase::addDatabase("QSQLITE");
    db2.setDatabaseName("\/tmp\/ass\/appinfo.db3");
    bool success = db2.open();
    QSqlQuery query1(db2);
     QString sqlStr1="select  count(*) from systemdb";
      query1.prepare(sqlStr1);
  if(query1.exec())
   {
      while(query1.next())
       {
           a=query1.value(0).toInt();
           break;
       }
   }
  ui->table_widget_third->setRowCount(a);
   for(int i=0;i<a;i++)
   {
       QSqlQuery query2(db2);
       QString sqlStr2="select appName,appsize,packageName from systemdb limit ";
       int m=i+1;

       char tmp[20];
      sprintf(tmp,"%d",m);
       sqlStr2 += tmp;
       query2.prepare(sqlStr2);
        if(query2.exec())
        {
               while(query2.next())
               {
                   for(int j=2;j<5;j++)
                   {
                       app_table_widget_item[i][j]=new QTableWidgetItem;
                       ui->table_widget_third->setItem(i,j,app_table_widget_item[i][j]);
                       if(j != 3)
                       {
                                   app_table_widget_item[i][j]->setText(query2.value(j-2).toString());
                        }else
                         if(j == 3)
                          {
                                    int size = query2.value(1).toInt();
                                   QString appstr = formatsize(size);
                                   app_table_widget_item[i][j]->setText(appstr);
                              }
                        app_table_widget_item[i][j]->setTextAlignment(Qt::AlignVCenter);
                   }
            }
       }
        QLabel *label1 = new QLabel;
        label1->clear();
        label1->setPixmap(QPixmap("/opt/ass/pics/sys-icon.png"));
        label1->setScaledContents(true);
        ui->table_widget_third->setCellWidget(i,1,label1);
   }

}
//获得app复选框的位置
void MainWindow::goto_check(int row, int col)
{

    qDebug()<<row;
    if(col != 1)
    {
        QSqlQuery query4(db1);
        QString sqlstr4 = "select apppath from thirddb limit ";
        char tmp3[30];
        sprintf(tmp3,"%d",row);
        sqlstr4 = sqlstr4 + tmp3 + ","+ "1";
        qDebug()<<sqlstr4;
        query4.exec(sqlstr4);
    if(ui->table_widget_third->item(row,col)->checkState() == Qt::Checked)
    {
        Appline_count<<QString::number(row);
        if(query4.first())
        {
            list<<query4.value(0).toString();//将复选框选中行的app路径信息放入list数组中
        }
        appunlist<<ui->table_widget_third->item(row,4)->text();//将复选框选中行的包名信息放入appunlist数组中
        appback<<ui->table_widget_third->item(row,4)->text();
    }
    else
    {
        if(query4.first())
        {
            list.removeOne(query4.value(0).toString());//将复选框选中行的app路径信息放入list数组中
        }
        appunlist.removeOne(ui->table_widget_third->item(row,4)->text());
        appback.removeOne(ui->table_widget_third->item(row,4)->text());
        Appline_count.removeOne(QString::number(row));
    }
       qDebug()<<list;
       qDebug()<<appunlist;
       qDebug()<<Appline_count;
    }
   // db1.close();
}

//从本地安装app到手机上
void MainWindow::goto_appinstall()
{

    QFileDialog *fd = new QFileDialog(this);
    fd->setWindowTitle("select a apk from the file ");
    fd->setGeometry(200,200,600,400);
    fd->setDirectory("/home");
    fd->setNameFilter(tr("apk Files(*.apk)"));

    if (fd->exec()==QFileDialog::Accepted)// ok
    {
        QTextStream cout(stdout, QIODevice::WriteOnly);

        QStringList slist=fd->selectedFiles();
        QString list = slist[0];
        QString appstr;

         appstr ="/opt/ass/bin/adb install "+ list;
         local_appinstall = new Install_app(appstr,this);
         local_appinstall->start();

         connect(local_appinstall,SIGNAL(install_finished(QString)),this,SLOT(appinstall_finished(QString)));
    }

}

void MainWindow::appinstall_finished(QString local_result)
{
    if(local_result.contains("Success"))
    {
      QMessageBox::about(this, tr(" "),tr("安装成功"));
        on_pushButton_4_clicked();
    }
    else
        QMessageBox::about(this,tr(""),tr("安装失败  "));
}
//将选中的app导出到本地
void MainWindow::goto_appbackup()
{
    QFile appfile;
    appfile.setFileName(QFileDialog::getExistingDirectory(this, tr("open Directory"), "/home", QFileDialog::DontResolveSymlinks));//文件对话框选择apk将要保存的文件夹
    qDebug()<<appfile.fileName();
    if(appfile.fileName().isEmpty())
         return;
    else
    {
        QString path = appfile.fileName();
        QString appcmd;
        for(int j =0; j < appback.size();++j)
        {
           QProcess *appbackup = new QProcess;
           qDebug()<<"appunlist is "<<appback.at(j);
           appcmd = "/opt/ass/bin/adb pull "+list.at(j)+" "+ path+"/"+appback.at(j)+".apk";
           appbackup->start(appcmd);
           //appbackup->waitForFinished();
           qDebug()<<appcmd;
        }
    }
    list.clear();
    appback.clear();
    appunlist.clear();
    Appline_count.clear();
    qDebug()<<"the path is "<<list;
    qDebug()<<"the appback packagename is "<<appback;
    qDebug()<<"the appuninstall packagename is "<<appunlist;
    qDebug()<<"the select row is "<<Appline_count;
    int count = ui->table_widget_third->rowCount();
    for(int app_thirdcheck =0;app_thirdcheck< count;app_thirdcheck++)
    {
        ui->table_widget_third->item(app_thirdcheck,0)->setCheckState(Qt::Unchecked);
    }
}
//pc端卸载手机端app
void MainWindow::goto_appuninstall()
{
    QFile appuninstall;
    int ok = QMessageBox::warning(this, tr("卸载应用 "),tr("are you sure"),QMessageBox::Yes,QMessageBox::No);

    if(ok == QMessageBox::Yes)
    {
        for(int i =0; i < appunlist.size();++i)
        {
            QProcess *appun = new QProcess;
            QString appuncmd = "/opt/ass/bin/adb uninstall " + appunlist.at(i);
            appun->start(appuncmd);
         //   appun->waitForFinished();
        }
        for(int app_tmp =0; app_tmp < Appline_count.size();++app_tmp)
        {
             int appcheck_row = Appline_count.at(app_tmp).toInt();
              ui->table_widget_third->item(appcheck_row,0)->setCheckState(Qt::Unchecked);
             ui->table_widget_third->removeRow(appcheck_row);
        }
        QMessageBox::about(this, tr(" "),tr("卸载成功"));
    }
     appunlist.clear();
     list.clear();
     appback.clear();
     Appline_count.clear();
     qDebug()<<"the path is "<<list;
     qDebug()<<"the appback packagename is "<<appback;
     qDebug()<<"the appuninstall packagename is "<<appunlist;
     qDebug()<<"the select row is "<<Appline_count;
   on_pushButton_4_clicked();
}
//加载相册
void MainWindow::LoadImage()
{
   QListWidget *image_list = new QListWidget(ui->tab_8);
   image_list->show();
   image_list->setGeometry(QRect(20,0,781,520));
   listWidget_File = new QListWidget(image_list);
   listWidget_File->setObjectName(QString::fromUtf8("listWidget_File"));
   listWidget_File->setGeometry(QRect(0, 0, 0, 0));
   QDirIterator m_DirIterator(QString("/tmp/ass/imageThumbnail"),QDir::Files|QDir::NoSymLinks,QDirIterator::Subdirectories);
   listWidget_File->setSortingEnabled(true);
   listWidget_File->clear();//保证每次进入listWidget_File的时候都会是空的
    while (m_DirIterator.hasNext())
    {
       QString tempFile=m_DirIterator.next();
         listWidget_File->setIconSize(QSize(120,120));       //设置QListWidget中的单元项的图片大小
      //   listWidget_File->setResizeMode(QListView::Adjust);
         listWidget_File->setViewMode(QListView::IconMode);   //设置QListWidget的显示模式
        // listWidget_File->setMovement(QListView::Static);    //设置QListWidget中的单元项不可被拖动
         listWidget_File->setSpacing(10);                   //设置QListWidget中的单元项的间距
          QPixmap objPixmap(tempFile);                      //生成图像objPixmap
                                                           //tempfile当中的/media/sd/PICTURES字符串删去，并返回剩余部分
          tempFile=tempFile.remove(QString("/tmp/ass/imageThumbnail/"), Qt::CaseSensitive);
          //生成QListWidgetItem对象

           QListWidgetItem *pItem = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(100,100))),NULL);

           pItem->setSizeHint(QSize(120,150));            //设置单元项的宽度和高度
           listWidget_File->addItem(pItem);              //添加QListWidgetItem项

         QCheckBox *imageCheckBox = new QCheckBox(this);
         imageCheckBox->setObjectName(QStringLiteral("%1").arg(tempFile));

         int image_tmp = tempFile.indexOf(".");
         tempFile.truncate(image_tmp);
         int image_count = tempFile.toInt();


         imageCheckBox->setText(QStringLiteral("%1").arg(uiimageinfo.image_item[image_count-1].img_name));
         listWidget_File->setItemWidget(pItem,imageCheckBox);
         connect(imageCheckBox,SIGNAL(stateChanged(int)),this,SLOT(imagestateChanged(int)));
     }
          listWidget_File->setGeometry(NULL,NULL,820,550);
          listWidget_File->show();
          connect(ui->Btn_imageBackup,SIGNAL(clicked()),this,SLOT(goto_imagebackup()));


          if(showload_image != NULL)
                      showload_image->close();
  }
//检测相册复选框状态的变化
void MainWindow::imagestateChanged(int state)
{
    QCheckBox *pSendCheckBox = qobject_cast<QCheckBox*>(sender());
    int   nCount =listWidget_File->count();

    for(int i =0; i<nCount; ++i)
    {
        QString strText;
        QListWidgetItem *pItem = listWidget_File->item(i);
        QWidget *pWidget = listWidget_File->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if(pCheckBox->isChecked())
        {
                if(pSendCheckBox == pCheckBox)
                {
                    strText = pCheckBox->objectName();
                      //  strText = pCheckBox->text();
                        int imagec = strText.indexOf(".");
                        strText.truncate(imagec);
                        imagelist<<strText;
                }
        }
        else
        {
            strText = pCheckBox->objectName();
          //  strText = pCheckBox->text();
            int imagec = strText.indexOf(".");
            strText.truncate(imagec);
            imagelist.removeOne(strText);
        }

    }
     qDebug()<<imagelist;
}
//对复选框选中的图片进行备份
void MainWindow::goto_imagebackup()
{
    QFile imagefile;
    imagefile.setFileName(QFileDialog::getExistingDirectory(this, tr("open Directory"), "/home", QFileDialog::DontResolveSymlinks));
     qDebug()<<imagefile.fileName();
     if(imagefile.fileName().isEmpty())
         return;
     else
     {
         QString localpath = imagefile.fileName();
         QString imagecmd;
         qDebug()<<imagelist.size();
         for(int image_count = 0; image_count < imagelist.size();++image_count)
         {
             QProcess *imagebacpup = new QProcess;
             int image_backup = imagelist.at(image_count).toInt();
             QString filepath; //= uiimageinfo.image_item[image_backup].img_path;

             qDebug()<<"you select the backup image is "<<uiimageinfo.image_item[image_backup-1].img_path;
             char resstr[100] = "/storage/emulated/legacy/";
              int res = strncmp(uiimageinfo.image_item[image_backup-1].img_path,"/storage/emulated/0",19);
             if (res == 0)
             {
                  strcat(resstr, uiimageinfo.image_item[image_backup-1].img_path+20);
                  filepath = resstr;
             }
             else
                 filepath = uiimageinfo.image_item[image_backup-1].img_path;
            imagecmd ="/opt/ass/bin/adb pull "+filepath+" "+localpath;
             qDebug()<<imagecmd;
             imagebacpup->start(imagecmd);
           }
        // int imagecheck_count = listWidget_File->count();
        // for(int image =0; image < imagecheck_count; image++)
          //   listWidget_File->item(image)->setCheckState(Qt::Unchecked);

     }
     imagelist.clear();
}
//发送获取相册的指令
void MainWindow::goto_image()
{
    post_refresh(CMD_IMAGE);
    QMovie *mov = new QMovie(":/img/pics/1.gif");
     showload_image->setMovie(mov);
     showload_image->setGeometry(360,160,300,300);
     showload_image->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
     showload_image->show();
     mov->start();

}
//接收到收到图片的指令
void MainWindow::setui_image()
{
    memset(&uiimageinfo,0,sizeof(image_pack_t));
    get_image(&uiimageinfo);
    LoadImage();
}
//加载音乐
void MainWindow::LoadAudio()
{
      ui->music_widget->clear();
      // Audio_table = new QTableWidget(ui->music_widget);
      //ui->music_widget->setGeometry(0,40,850,531);

      ui->music_widget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-image:url(:/img/pics/bg2.png);border-image:url(:/img/pics/bg2.png);color:black;}");
      ui->music_widget->setFrameShape(QFrame::NoFrame);
       ui->music_widget->setShowGrid(false);
      ui->music_widget->setMouseTracking(true);
      ui->music_widget->verticalHeader()->setDefaultSectionSize(48);
      ui->music_widget->setSelectionMode(QAbstractItemView::SingleSelection);
       ui->music_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
     //  ui->music_widget->setAlternatingRowColors(true);

        ui->music_widget->verticalHeader()->setVisible(false);;
        ui->music_widget->horizontalHeader()->setVisible(true);

       ui->music_widget->setColumnCount(5);
      ui->music_widget->setColumnWidth(0,30);
      ui->music_widget->setColumnWidth(1,200);
        ui->music_widget->setColumnWidth(2,180);
        ui->music_widget->setColumnWidth(3,200);
        ui->music_widget->setColumnWidth(4,153);
        //ui->music_widget->setAlternatingRowColors(true);

        QStringList label3;
        label3<<tr("")<<tr("标题")<<tr("时长")<<tr("艺术家")<<tr("大小");
        ui->music_widget->setHorizontalHeaderLabels(label3);
        connect(ui->music_widget,SIGNAL(cellClicked(int,int)),this,SLOT(Audio_clicked(int,int)));//获取点击位置
        connect(ui->Btn_audio,SIGNAL(clicked()),this,SLOT(goto_audio()));//发送获取音频文件的指令
        connect(ui->music_widget,SIGNAL(cellChanged(int,int)),this,SLOT(goto_checkaudio(int,int)));
        connect(ui->Btn_audioBackup,SIGNAL(clicked()),this,SLOT(goto_audiobackup()));
}
//发送获取音乐的指令
void MainWindow::goto_audio()
{
    post_refresh(CMD_AUDIO);
    QMovie *mov = new QMovie(":/img/pics/1.gif");
        showload_audio->setMovie(mov);
        showload_audio->setGeometry(360,160,300,300);
        showload_audio->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
       showload_audio->show();
        mov->start();
}
//获取点击音乐的位置
void MainWindow::Audio_clicked(int x,int y)
{
     AUDIOXSIGN = x;
}
//得到音乐显示
void MainWindow::setui_audio()
{
        memset(&uiaudioinfo,0,sizeof(audio_pack_t));
        get_audio(&uiaudioinfo);
        ui->music_widget->setRowCount(0);
        ui->music_widget->setRowCount(uiaudioinfo.count+1);
        qDebug()<<"the music count is "<<uiaudioinfo.count;
        for(int i = 0; i<  uiaudioinfo.count;i++)
        {
            for(int j = 1; j < 5; j++)
            {
                Audio_item[i][j] = new QTableWidgetItem;
                Audio_item[i][j]->setTextAlignment(Qt::AlignCenter);
                ui->music_widget->setItem(i,j,Audio_item[i][j]);
            }
            Audio_item[i][1]->setText(uiaudioinfo.audio_item[i].audio_title);

            int dursize = uiaudioinfo.audio_item[i].audio_duration;
            QString durstr = formattime(dursize);
            Audio_item[i][2]->setText(durstr);

            Audio_item[i][3]->setText(uiaudioinfo.audio_item[i].audio_artist);

            int size = uiaudioinfo.audio_item[i].audio_size;
            QString datastr = formatsize(size);
            Audio_item[i][4]->setText(datastr);
            Audio_item[i][4]->setTextAlignment(Qt::AlignVCenter);

          QTableWidgetItem *checkBox = new QTableWidgetItem();
          // QCheckBox *checkBox = new QCheckBox(this);
            //checkBox->setBackground(QColor(0,60,10));
            checkBox->setCheckState(Qt::Unchecked);
            ui->music_widget->setItem(i,0,checkBox);
        }
     showload_audio->close();
}
//检测音乐文件复选框选中的位置
void MainWindow::goto_checkaudio(int row,  int col)
{
    if(ui->music_widget->item(row,col)->checkState() == Qt::Checked)
    {
           // musiclist<<uiaudioinfo.audio_item[row].audio_path;
           //QProcess:execuate("${uiaudioinfo.audio_item[row].audio_path/"/0/music"/"/legacy/music"}");
           char resstr[100] = "/storage/emulated/legacy/";
            int res = strncmp(uiaudioinfo.audio_item[row].audio_path,"/storage/emulated/0",19);
           if (res == 0)
           {
                strcat(resstr, uiaudioinfo.audio_item[row].audio_path+20);
                musiclist<<resstr;
          }
           else
               musiclist<<uiaudioinfo.audio_item[row].audio_path;
    }
    else
    {
        char resstr[120] = "/storage/emulated/legacy/";
         int res = strncmp(uiaudioinfo.audio_item[row].audio_path,"/storage/emulated/0",19);
        if (res == 0)
        {
             strcat(resstr, uiaudioinfo.audio_item[row].audio_path+20);
            musiclist.removeOne(resstr);
       }
        else
            musiclist.removeOne(uiaudioinfo.audio_item[row].audio_path);
    }
       qDebug()<<musiclist;
}
//将手机音乐备份到本地
void MainWindow::goto_audiobackup()
{
    QFile musicfile;
     musicfile.setFileName(QFileDialog::getExistingDirectory(this, tr("open Directory"), "/home", QFileDialog::DontResolveSymlinks));
     qDebug()<<musicfile.fileName();
     if(musicfile.fileName().isEmpty())
         return;
     else
     {
         QString localpath = musicfile.fileName();
         QString musiccmd;
         qDebug()<<musiclist.size();
         for(int music_count = 0; music_count < musiclist.size();++music_count)
         {
             QProcess *musicbacpup = new QProcess;
             QString musicstr;
            musicstr = QString("\"")+musiclist.at(music_count)+QString("\"");

             musiccmd = "/opt/ass/bin/adb pull "+musicstr+" "+localpath;
             musicbacpup->start(musiccmd);
            // QProcess::execute(musiccmd);
         }
         musiclist.clear();
         for(int i= 0;i < uiaudioinfo.count;i++)
         {
           ui->music_widget->item(i,0)->setCheckState(Qt::Unchecked);
         }
     }
}
//将时间格式转换
QString MainWindow::formattime(int time)
{
    QString realtime;
    int hour = 0;
    int minute = 0;
    int second = 0;

    second = time / 1000;
    if(second >60)
    {
        minute = second /60;
        second = second % 60;
    }
    if(minute > 60)
    {
        hour = minute / 60;
        minute = minute % 60;
    }
   qDebug()<<hour<<","<<minute<<","<<second;
  // realtime.sprintf("%2d:%2d:%2d",hour,minute,second);
  realtime = QString("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(minute,2,10,QChar('0')).arg(second,2,10,QChar('0'));

   return realtime;
}
//占用内存大小格式转换
QString MainWindow::formatsize(int size)
{
    QString realsize;
    float sizef = (float)size /(float)1000000;
    realsize = QString("%1M").arg(sizef,'0','f',2);
    return realsize;
}
//加载视频文件
void MainWindow::setui_video()
{
    memset(&uivideoinfo,0,sizeof(video_pack_t));
    get_video(&uivideoinfo);
    LoadVideo();
}
//发送获取视频的指令
void MainWindow::goto_video()
{
    post_refresh(CMD_VIDEO);

    QMovie *mov = new QMovie(":/img/pics/1.gif");
         showload_video->setMovie(mov);
         showload_video->setGeometry(360,160,300,300);
         showload_video->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
         showload_video->show();
         mov->start();
}
//加载视频
void MainWindow::LoadVideo()
{
    QListWidget *video_list = new QListWidget(ui->tab_4);
    video_list->show();
    video_list->setGeometry(QRect(20,0,781,500));
    list_widgetvideo = new QListWidget(video_list);
    list_widgetvideo->setGeometry(QRect(0,0,0,0));
     QDirIterator video_Dir(QString("/tmp/ass/videoThumbnail"),QDir::Files|QDir::NoSymLinks,QDirIterator::Subdirectories);
    list_widgetvideo->clear();

  while(video_Dir.hasNext())
  {
      QString tempfile = video_Dir.next();
      list_widgetvideo->setIconSize(QSize(150, 150));        //设置QListWidget中的单元项的图片大小
      list_widgetvideo->setResizeMode(QListView::Adjust);
      list_widgetvideo->setViewMode(QListView::IconMode);   //设置QListWidget的显示模式
      list_widgetvideo->setMovement(QListView::Static);    //设置QListWidget中的单元项不可被拖动
      list_widgetvideo->setSpacing(10);                   //设置QListWidget中的单元项的间距
      QPixmap objPixmap(tempfile);

      tempfile=tempfile.remove(QString("/tmp/ass/videoThumbnail/"), Qt::CaseSensitive);
      //生成QListWidgetItem对象
      QListWidgetItem *pItem = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(100,100))),NULL);
      pItem->setSizeHint(QSize(150,170));            //设置单元项的宽度和高度
      list_widgetvideo->addItem(pItem);              //添加QListWidgetItem项

         QCheckBox *videoCheckBox = new QCheckBox(this);
        videoCheckBox->setObjectName(QStringLiteral("%1").arg(tempfile));

        int video_tmp = tempfile.indexOf(".");
        tempfile.truncate(video_tmp);
        int video_count =tempfile.toInt();

        int videosize = uivideoinfo.video_item[video_count-1].video_size;
        QString videosize_str = formatsize(videosize);
        videosize_str =videosize_str.simplified();
        qDebug()<<videosize_str;

         videoCheckBox->setText(QStringLiteral("%1\n%2").arg(uivideoinfo.video_item[video_count-1].video_title).arg(videosize_str));
         list_widgetvideo->setItemWidget(pItem,videoCheckBox);
        connect(videoCheckBox,SIGNAL(stateChanged(int)),this,SLOT(videostateChanged(int)));

  }
  list_widgetvideo->setGeometry(NULL,NULL,820,500);
  list_widgetvideo->show();
  connect(ui->Btn_videBackup,SIGNAL(clicked()),this,SLOT(goto_videobackup()));


  showload_video->close();
}
//检测视频复选框状态的变化
void MainWindow::videostateChanged(int state)
{
    QCheckBox *pSendCheckBox = qobject_cast<QCheckBox*>(sender());
    int   nCount =list_widgetvideo->count();

    for(int i =0; i<nCount; ++i)
    {
        QString strText;
        QListWidgetItem *pItem =list_widgetvideo ->item(i);
        QWidget *pWidget = list_widgetvideo->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if(pCheckBox->isChecked())
        {
                if(pSendCheckBox == pCheckBox)
                {
                        strText = pCheckBox->objectName();
                       // strText = pCheckBox->text();
                        int imagec = strText.indexOf(".");
                        strText.truncate(imagec);
                        videolist<<strText;
                        qDebug()<<"you select the video is "<<videolist;
                }
        }
        else
        {
            qDebug()<<"qcheck box is running";
            strText = pCheckBox->objectName();
          // strText = pCheckBox->text();
            int imagec = strText.indexOf(".");
            strText.truncate(imagec);
            videolist.removeOne(strText);
        }
    }
     qDebug()<<"you select the video is "<<videolist;
}
//对复选框选中的视屏进行备份
void MainWindow::goto_videobackup()
{
    QFile videofile;
    videofile.setFileName(QFileDialog::getExistingDirectory(this, tr("open Directory"), "/home", QFileDialog::DontResolveSymlinks));
     qDebug()<<videofile.fileName();
     if(videofile.fileName().isEmpty())
         return;
     else
     {
         QString localpath = videofile.fileName();
         QString videocmd;
         qDebug()<<videolist.size();
         for(int video_count = 0; video_count < videolist.size();++video_count)
         {
             QProcess *videobackup = new QProcess;
             int video_backup =videolist.at(video_count).toInt();
             QString filepath; //= uiimageinfo.image_item[image_backup].img_path;

             char resstr[100] = "/storage/emulated/legacy/";
              int res = strncmp(uivideoinfo.video_item[video_backup-1].video_path,"/storage/emulated/0",19);
             if (res == 0)
             {
                  strcat(resstr, uivideoinfo.video_item[video_backup-1].video_path+20);
                  filepath = resstr;
             }
             else
                 filepath = uivideoinfo.video_item[video_backup-1].video_path;
             videocmd ="/opt/ass/bin/adb pull "+filepath+" "+localpath;
             qDebug()<<videocmd;
             videobackup->start(videocmd);
           }
            videolist.clear();
     }
}
//后台程序初始化
void MainWindow::BackThreadinit()
{
    Backthread = new BackThread(this);
    QThread* thread = new QThread;
    Backthread->moveToThread(thread);
    thread->start();
}
//设备程序初始化
void MainWindow::DaemonThreadinit()
{
    DaemonThread = new daemon(this);
    QThread* daemon_thread = new QThread;
    DaemonThread->moveToThread(daemon_thread);
    daemon_thread->start();
    connect(DaemonThread,SIGNAL(connect_finished()),this,SLOT(connect_finished()));
    post_daemon();
}
void MainWindow::connect_finished()
{
     connect_init();
     //screenshotinit();
     Disconnect_Thread =new Disconnect_thread;//新建线程
     connect(Disconnect_Thread,SIGNAL(msgSignal(int)),this,SLOT(display(int)));//连接线程抛出的信号
     Disconnect_Thread->start();//启动线程
}
void MainWindow::post_daemon()
{
    ASSEvent* event_daemon = new ASSEvent(ASSEVENT_DAEMON,0);
    QApplication::postEvent(DaemonThread,event_daemon);
}
//给后台程序发送刷新的事件,通知后台程序需要发送给手机端的指令（eg.contacts,mms）
void MainWindow::post_refresh(int cmd)
{
    ASSEvent  *event_refresh = new ASSEvent(ASSEVENT_REFRESH, cmd);
    QApplication::postEvent(Backthread,event_refresh);
}
//接收其他线程发给MainWindow 的事件，并执行相应的函数
void MainWindow::customEvent(QEvent *e)
{
    ASSEvent* jpe = dynamic_cast<ASSEvent*>(e);
    switch (jpe->type())
    {
        case ASSEVENT_REFRESH:
            recv_refresh(jpe->msg());
            break;
        default:
            break;
        }
}
//根据接收到的内容，将其显示在相应的位置
void MainWindow::recv_refresh(int res)
{
    switch(res)
    {
       case  CMD_CONTACTS:
            setui_contacts();
        break;
    case CMD_MESSAGE:
                setui_mmsinfo();
      break;
    case CMD_APPINFO:
        setui_app(APPTYPE);
        ui->cmb_app->setCurrentIndex(APPTYPE);
       break;
    case CMD_IMAGE:
        setui_image();
        break;
    case CMD_AUDIO:
        setui_audio();
        break;
    case  CMD_VIDEO:
        setui_video();
        break;
    case CMD_RECV_MESSAGE:
        setui_recvmms();
        break;
    default:
        break;
    }

}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    last = e->globalPos();
    move(x() + dx, y() + dy);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() -last.x();
    int dy = e->globalY() - last.y();

    move(x() + dx,y() + dy);
}
//显示联系人
void MainWindow::on_getlist_clicked()
{
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->listWidget->show();//使用列表
    ui->tableView->hide();

    QMovie *mov = new QMovie(":/img/pics/1.gif");
          showload_contacts->setMovie(mov);
          showload_contacts->setGeometry(360,160,300,300);
          showload_contacts->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
          showload_contacts->show();
           mov->start();

    ui->tableView->hideColumn(0);
    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->setVisible(false);
    //ui->tableView->setAlternatingRowColors(true);  //奇偶行底色不同
    ui->tableView->setFrameShape(QFrame::NoFrame);  //无边框
    ui->tableView->verticalHeader()->setDefaultSectionSize(30);
    ui->tableView->setRowHeight(2,200);

    ui->getlist->setStyleSheet("QPushButton{border-image: url(:/img/pics/sx2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_sx2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_sx2.png);}");
   post_refresh(CMD_CONTACTS);
}
//联系人读取并显示
void MainWindow::setui_contacts()
{
    qDebug()<<"setui contacts is running ";
              // setui_clean();
    QSqlDatabase db;
      if(QSqlDatabase::contains("qt_sql_default_connection"))
          db = QSqlDatabase::database("qt_sql_default_connection");
      else
          db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName("\/tmp\/ass\/contacts.db3");
      bool success = db.open();

      int a;//get the num
      QSqlQuery query1(db);
      QString sqlStr1="select  count(*) from contactdb";
      query1.prepare(sqlStr1);
      if(query1.exec())
      {
          while(query1.next())
          {
              a=query1.value(0).toInt();
              break;
          }
      }
    ui->listWidget->clear();
      for(int i=0;i<a;i++)
      {
          QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
          ui->listWidget->addItem(item);
          CustomWidgetItem *customItem = new CustomWidgetItem(ui->listWidget->row(item), item, this);

          connect(customItem, SIGNAL(signalMeClicked(QString,QString,int)), this, SLOT(slotItemClicked(QString,QString,int)));
          item->setSizeHint(customItem->size());
          ui->listWidget->setItemWidget(item, customItem);
      }

    ui->getlist->setStyleSheet("QPushButton{border-image: url(:/img/pics/sx2.png);background-image : url(:/img/pics/nobg.png)}"
                     "QPushButton:hover{border-image: url(:/img/pics/h_sx2.png);}"
                     "QPushButton:pressed{border-image: url(:/img/pics/h_sx2.png);}");
    if(showload_contacts != NULL)
           showload_contacts->close();
    db.close();
}

void MainWindow::slotComboboxActivated(int index)
{
    QString text = qobject_cast<CustomWidgetItem *>(comboboxListWidget->itemWidget(comboboxListWidget->item(index)))->customText;
    comboboxListWidget->item(index)->setText(text);
}

void MainWindow::contact_sendmms()
{
    contact_mmsdialog = new MMS_Dialog;
    contact_mmsdialog->show();

  contact_mmsdialog->sendnumber(contact_db_text);
  qDebug()<<contact_db_text;
  connect(this->contact_mmsdialog,SIGNAL(mms_accept()),this, SLOT(get_contactmms()));
}

void MainWindow::get_contactmms()
{
    memset(&sendmms,'\0',sizeof(message_t));
    if((contact_mmsdialog->mmsnumber != "") && (contact_mmsdialog->mmsdata !="" ))
    {
        QByteArray number_list = contact_mmsdialog->mmsnumber.toLatin1();
        char *char_mmsnumber = number_list.data();

        QByteArray data_list = contact_mmsdialog->mmsdata.toUtf8();
        char *char_mmsdata = data_list.data();
        strcpy(sendmms.message_num,char_mmsnumber);
        strcpy(sendmms.message_body,char_mmsdata);

       set_message_cmd(&sendmms);//将号码和内容拷贝进将要发送的结构体
       post_refresh(CMD_SEND_MESSAGE);//给手机端发送发送短信的指令
    }
}
   //传递数据
void MainWindow::slotItemClicked(QString customTitle,QString customText,int index)
{
    contact_db_index=index;
    contact_db_title=customTitle;
    contact_db_text=customText;


    ui->label_8->setStyleSheet("font: 16pt \"黑体\";");
    ui->label_9->setStyleSheet("font: 16pt \"黑体\";");
    ui->label_10->setStyleSheet("font: 16pt \"黑体\";");
    ui->label_11->setStyleSheet("font: 16pt \"黑体\";");
    ui->toolButton_13->setIcon(QIcon("/opt/ass/pics/people.png"));
    ui->toolButton_13->setIconSize(QSize(48, 48));
    ui->toolButton_13->setAutoRaise(true);
    ui->label_8->setText(customTitle);
    ui->label_9->setText(customText);
    ui->label_10->setText("姓名：");
    ui->label_11->setText("号码：");
     //QGridLayout *gridLayout = new QGridLayout(ui->gridLayout);
     ui->gridLayout->addWidget(ui->toolButton_13,0,0,1,1);
     ui->gridLayout->addWidget(ui->label_10,1,0,1,1);
     ui->gridLayout->addWidget(ui->label_8,1,1,1,1);
     ui->gridLayout->addWidget(ui->label_11,2,0,1,1);
     ui->gridLayout->addWidget(ui->label_9,2,1,1,1);
     this->setLayout(ui->gridLayout);

      ui->label_8->show();
      ui->label_9->show();
      ui->label_10->show();
      ui->label_11->show();
      ui->toolButton_13->show();

}

//视频页面设置
void MainWindow::on_toolButton_4_clicked()
{
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_home.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
    //                                "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
       //                             "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab->resize(900,600);
    ui->tab->show();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();
}

void MainWindow::on_toolButton_5_clicked()
{
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_txl.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
    //                                "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
     //                               "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_2->resize(900,600);
    ui->tab->hide();
    ui->tab_2->show();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();

    ui->label_2->hide();
    ui->label_5->hide();
    ui->label_4->hide();
    ui->lineEdit->hide();
    ui->lineEdit_3->hide();

    ui->label_8->hide();
    ui->label_9->hide();
    ui->label_10->hide();
    ui->label_11->hide();
    ui->toolButton_13->hide();


}
//安卓市场页面设置
void MainWindow::on_toolButton_6_clicked()
{
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_xx.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
       //                             "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
          //                          "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_3->resize(900,600);
    ui->tab->hide();
    ui->tab_2->hide();
    ui->tab_3->show();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();
}
//音乐页面设置
void MainWindow::on_toolButton_7_clicked()
{
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_movie.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
       //                             "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
          //                          "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_4->resize(900,600);
    ui->tab_4->show();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();
}
//相册页面设置
void MainWindow::on_toolButton_8_clicked()
{
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_yyrj.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
       //                             "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
          //                          "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_5->resize(900,600);
    ui->tab_5->show();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();
}
//电子书页面设置
void MainWindow::on_toolButton_9_clicked()
{
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_sc.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_6->resize(900,600);
    ui->tab_6->show();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();
}
//
void MainWindow::on_toolButton_clicked()
{
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_music.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
       //                             "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
          //                          "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_7->resize(900,600);
    ui->tab->hide();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->show();
    ui->tab_8->hide();
    ui->tab_9->hide();
    ui->tab_10->hide();

}

void MainWindow::on_toolButton_12_clicked()
{
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_pic.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
       //                             "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
          //                          "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_8->resize(900,600);
    ui->tab->hide();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->show();
    ui->tab_9->hide();
    ui->tab_10->hide();

}

/*void MainWindow::on_toolButton_13_clicked()
{
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_ebook.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/folder.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_folder.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_folder.png);}");
    ui->tab_9->resize(900,600);
    ui->tab->hide();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_9->show();
    ui->tab_10->hide();

}*/

//文件管理初始化
void MainWindow::on_toolButton_14_clicked()
{

    ui->toolButton_14->setStyleSheet("QToolButton{border-image: url(:/img/pics/p_folder.png);background-image : url(:/img/pics/nobg.png)}");
    ui->toolButton_4->setStyleSheet("QToolButton{border-image: url(:/img/pics/home.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_home.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_home.png);}");
    ui->toolButton_5->setStyleSheet("QToolButton{border-image: url(:/img/pics/txl.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_txl.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_txl.png);}");
    ui->toolButton_6->setStyleSheet("QToolButton{border-image: url(:/img/pics/xx.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_xx.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_xx.png);}");
    ui->toolButton_7->setStyleSheet("QToolButton{border-image: url(:/img/pics/movie.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_movie.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_movie.png);}");
    ui->toolButton_8->setStyleSheet("QToolButton{border-image: url(:/img/pics/yyrj.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_yyrj.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_yyrj.png);}");
    ui->toolButton_9->setStyleSheet("QToolButton{border-image: url(:/img/pics/sc.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_sc.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_sc.png);}");
    ui->toolButton->setStyleSheet("QToolButton{border-image: url(:/img/pics/music.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_music.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_music.png);}");
    ui->toolButton_12->setStyleSheet("QToolButton{border-image: url(:/img/pics/pic.png);background-image : url(:/img/pics/nobg.png)}"
                                    "QToolButton:hover{border-image: url(:/img/pics/h_pic.png);}"
                                    "QToolButton:pressed{border-image: url(:/img/pics/p_pic.png);}");
    //ui->toolButton_13->setStyleSheet("QToolButton{border-image: url(:/img/pics/ebook.png);background-image : url(:/img/pics/nobg.png)}"
       //                             "QToolButton:hover{border-image: url(:/img/pics/h_ebook.png);}"
          //                          "QToolButton:pressed{border-image: url(:/img/pics/p_ebook.png);}");
    ui->tab_10->resize(900,600);
    ui->tab->hide();
    ui->tab_2->hide();
    ui->tab_3->hide();
    ui->tab_4->hide();
    ui->tab_5->hide();
    ui->tab_6->hide();
    ui->tab_7->hide();
    ui->tab_8->hide();
    ui->tab_10->show();
    ui->tab_9->hide();
    ui->treeWidget->clear();
    ui->treeWidget_2->clear();
    ui->treeWidget->setColumnCount(1);
    QStringList headers;
    headers<<"目录";
    ui->treeWidget->header()->setStyleSheet("QHeaderView::section{background-image:url(:/img/pics/bg2.png);border-image:url(:/img/pics/bg2.png);color:black;}");
    ui->treeWidget->setHeaderLabels(headers);
    //sys_init();
    inside_init();

    FILE *fp_1;
    fp_1=popen("/opt/ass/bin/adb shell ls /storage/","r");
    char buf[50];
    while(!feof(fp_1))
    {
        memset(buf,'\0',50);
       if(fgets(buf,50,fp_1)==NULL)
           break;
       buf[strlen(buf)-1]=0;
       if(strncmp(buf,"ext_sd",6)==0)
       {
           outside_init();
       }
    }
    pclose(fp_1);

    ui->treeWidget_2->header()->setStyleSheet("QHeaderView::section{background-image:url(:/img/pics/bg2.png);border-image:url(:/img/pics/bg2.png);color:black;}");
    ui->treeWidget_2->setColumnCount(2);
    ui->treeWidget_2->setStyleSheet("background-image : url(:/img/pics/bg2.png)");
    QStringList header;
    header<<"文件名"<<"路径";
    ui->treeWidget_2->setHeaderLabels(header);
    ui->treeWidget_2->setColumnWidth(0,200);
    ui->treeWidget_2->setColumnWidth(1,500);
   // list->setGeometry(0,0,650,581);
}
//应用管理，内置存储卡目录列表显示
void MainWindow::inside_init()
{
    QTreeWidgetItem *inside_1=new QTreeWidgetItem(ui->treeWidget);

    inside_1->setText(0,"内置SD卡");
    inside_1->setText(1,"/storage/sdcard0/");
    char parent_path[1024]="/storage/sdcard0/";
    FILE *fp_1;
    fp_1=popen("/opt/ass/bin/adb shell ls /storage/sdcard0/","r");
    char buf[500];
    while(!feof(fp_1))
    {
        char path_2[500];
        memset(buf,'\0',500);
        memset(path_2,'\0',500);
       if(fgets(buf,500,fp_1)==NULL)
           break;
       buf[strlen(buf)-1]=0;
       strcat(path_2,parent_path);

       char new_buf[1024];
       memset(new_buf,'\0',1024);
       int j=0;
       for(int i=0;i<strlen(buf);i++)
       {
           if(buf[i]!=32)
           {
               new_buf[j++]=buf[i];
           }
           else
           {
               new_buf[j++]=92;
               new_buf[j++]=32;
           }
       }

       strcat(path_2,new_buf);
       QTreeWidgetItem *step_2=new QTreeWidgetItem(inside_1);
       step_2->setText(0,buf);
       step_2->setText(1,path_2);
       step_2->setIcon(0,QIcon(QPixmap(":/img/pics/dir.png")));

    }
    pclose(fp_1);
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showSelectedList(QTreeWidgetItem*)));
     connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showClickedList(QTreeWidgetItem*)));
    // connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(choose_dir(QTreeWidgetItem*)));
}

//外部SD卡
void MainWindow::outside_init()
{
    QTreeWidgetItem *outside_1=new QTreeWidgetItem(ui->treeWidget);
    outside_1->setText(0,"外部SD卡");
    outside_1->setText(1,"/storage/ext_sd/");
    char parent_path[24]="/storage/ext_sd/";
    FILE *fp_1;
    char path_1[2]="/";
    fp_1=popen("/opt/ass/bin/adb shell ls /storage/ext_sd/","r");
    char buf[500];
    while(!feof(fp_1))
    {
        char path_2[1024];
        memset(buf,'\0',500);
        memset(path_2,'\0',1024);
       if(fgets(buf,500,fp_1)==NULL)
           break;
       buf[strlen(buf)-1]=0;
       char new_buf[1024];
       memset(new_buf,'\0',1024);
       int j=0;
       for(int i=0;i<strlen(buf);i++)
       {
           if(buf[i]!=32)
           {
               new_buf[j++]=buf[i];
           }
           else
           {
               new_buf[j++]=92;
               new_buf[j++]=32;
           }
       }
       strcat(path_2,parent_path);
       strcat(path_2,new_buf);
       QTreeWidgetItem *step_2=new QTreeWidgetItem(outside_1);
       step_2->setText(0,buf);
       step_2->setText(1,path_2);
       step_2->setIcon(0,QIcon(QPixmap(":/img/pics/dir.png")));
    }
    pclose(fp_1);
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showClickedList(QTreeWidgetItem*)));
   // connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(choose_dir(QTreeWidgetItem*)));
}
//打开目录下一级
void MainWindow::showSelectedList(QTreeWidgetItem *item)
{
    QTreeWidgetItem *parent = item->parent();

    if(NULL==parent) //注意：最顶端项是没有父节点的，双击这些项时注意(陷阱)
        return;

    QString path1=parent->text(1);//father text
    QByteArray da=path1.toLocal8Bit();
    QString path2=item->text(1);
    QByteArray ba = path2.toLocal8Bit();
    char *nn= ba.data();
    nn[strlen(nn)-1]=0;
    char cmd[1024]="/opt/ass/bin/adb shell ls ";
    strcat(cmd,nn);
    FILE *fp;
    char path_1[2]="/";
    strcat(cmd,path_1);
    fp=popen(cmd,"r");
    char buf[1024];
    //qDebug()<<buf;
    while(!feof(fp))
    {

        char path_2[1024];
        memset(buf,'\0',1024);
        memset(path_2,'\0',1024);
       if(fgets(buf,1024,fp)==NULL)
           break;
       buf[strlen(buf)-1]=0;
       char new_buf[1024];
       memset(new_buf,'\0',1024);
       int j=0;
       for(int i=0;i<strlen(buf);i++)
       {
           if(buf[i]!=32)
           {
               new_buf[j++]=buf[i];
           }
           else
           {
               new_buf[j++]=92;
               new_buf[j++]=32;
           }
       }
       strcpy(path_2,nn);
       strcat(path_2,path_1);
       int len1=strlen(path_2);
       //显示文本处理
      char *check="opendir failed, Permission denied";

      if(strncmp(buf,check,strlen(check))==0)//打开无权限
      {
           QMessageBox::warning(this,"Attention","Opendir failed, Permission denied");
      }
      else{
         if(strncmp(path_2,new_buf,len1-1)==0)//文本文件
         {
                 QMessageBox::warning(this,"Attention","This is a file,not dir");
           }
           else {
             //目录文件
               strcat(path_2,new_buf);

             QTreeWidgetItem *step_2=new QTreeWidgetItem(item);
             step_2->setText(0,buf);
             step_2->setText(1,path_2);
             step_2->setIcon(0,QIcon(QPixmap(":/img/pics/dir.png")));
             //connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(choose_dir(QTreeWidgetItem*)));
           }
      }

    }
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showClickedList(QTreeWidgetItem*)));

    pclose(fp);
}
//单击显示文件列表
void MainWindow::showClickedList(QTreeWidgetItem *item)
{
    ui->treeWidget_2->clear();
    int count=0;

    /*QTreeWidget *list=new QTreeWidget(ui->treeWidget_2);

    list->setColumnCount(2);
    list->setStyleSheet("background-image : url(:/img/pics/bg2.png)");
    QStringList header;
    header<<"文件名"<<"路径";
    list->setHeaderLabels(header);
    list->setColumnWidth(0,200);
    list->setColumnWidth(1,500);
    list->setGeometry(0,0,650,581);*/
    //ui->treeWidget_2->set
    QTreeWidgetItem *parent = item->parent();

    QString path1;
    QString firstname;
    firstname = item->text(0);
    QByteArray ha=firstname.toLocal8Bit();
    char *name=ha.data();
    name[strlen(name)-1]=0;


    if(NULL==parent) //注意：最顶端项是没有父节点的，双击这些项时注意(陷阱)
    {
         path1=item->text(1);
        QByteArray da=path1.toLocal8Bit();
        char *mm=da.data();
        mm[strlen(mm)-1]=0;

        char cmd[500]="/opt/ass/bin/adb shell ls ";
        strcat(cmd,mm);
        FILE *fp;

        char path_1[2]="/";
         strcat(cmd,path_1);
        fp=popen(cmd,"r");
        char buf[1024];
        while(!feof(fp))
        {
           char path_2[1024];
           memset(buf,'\0',1024);
           memset(path_2,'\0',1024);
           if(fgets(buf,1024,fp)==NULL)
                 break;
           buf[strlen(buf)-1]=0;
           char new_buf[1024];
           memset(new_buf,'\0',1024);
           int j=0;
           for(int i=0;i<strlen(buf);i++)
           {
               if(buf[i]!=32)
               {
                   new_buf[j++]=buf[i];
               }
               else
               {
                   new_buf[j++]=92;
                   new_buf[j++]=32;
               }
           }
           strcat(path_2,mm);
           strcat(path_2,path_1);
           strcat(path_2,new_buf);

          QTreeWidgetItem *step_2=new QTreeWidgetItem(ui->treeWidget_2);
           step_2->setText(0,buf);
           step_2->setText(1,path_2);
           step_2->setIcon(0,QIcon(QPixmap(":/img/pics/dir.png")));
           // list->show();

         }
        pclose(fp);
    }
    else
    {
        path1=parent->text(1);//father text

        QByteArray da=path1.toLocal8Bit();
        char *mm=da.data();
        mm[strlen(mm)-1]=0;
        QString path2=item->text(1);

        QByteArray ba = path2.toLocal8Bit();
        char *nn= ba.data();
         nn[strlen(nn)-1]=0;

         char cmd[500]="/opt/ass/bin/adb shell ls ";
         strcat(cmd,nn);
         FILE *fp;
         char path_1[2]="/";

         fp=popen(cmd,"r");
         char buf[1024];
         while(!feof(fp))
         {

            char path_2[1024];
            memset(buf,'\0',1024);
            memset(path_2,'\0',1024);
            if(fgets(buf,1024,fp)==NULL)
                  break;
            buf[strlen(buf)-1]=0;
            char new_buf[1024];
            memset(new_buf,'\0',1024);
            int j=0;
            for(int i=0;i<strlen(buf);i++)
            {
                if(buf[i]!=32)
                {
                    new_buf[j++]=buf[i];
                }
                else
                {
                    new_buf[j++]=92;
                    new_buf[j++]=32;
                }
            }
            strcpy(path_2,nn);
            strcat(path_2,path_1);
            strcat(path_2,new_buf);
            int saby=strlen(nn);
            //显示文本处理
            char *check="opendir failed, Permission denied";

            if(strncmp(buf,check,strlen(check))==0)//打开无权限
            {

                 QTreeWidgetItem *step_2=new QTreeWidgetItem(ui->treeWidget_2);
                 step_2->setText(0,name);
                 step_2->setText(1,nn);
                 step_2->setIcon(0,QIcon(QPixmap(":/img/pics/dir.png")));

                // list->show();

            }
            else{
             if(strncmp(nn,buf,saby)==0)
             {
                 QTreeWidgetItem *step_2=new QTreeWidgetItem(ui->treeWidget_2);
                 step_2->setText(0,name);
                 step_2->setText(1,nn);
                 step_2->setIcon(0,QIcon(QPixmap(":/img/pics/txt.png")));
                 //list->show();
              }else{
                QTreeWidgetItem *step_2=new QTreeWidgetItem(ui->treeWidget_2);
                 step_2->setText(0,buf);
                 step_2->setText(1,path_2);
                 step_2->setIcon(0,QIcon(QPixmap(":/img/pics/dir.png")));
                // list->show();
                  }
            }
          }
         pclose(fp);
    }

    connect(ui->treeWidget_2,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(choose_dir(QTreeWidgetItem*)));

}

void MainWindow::choose_dir(QTreeWidgetItem *item)
{
    QString path1;
    QString firstname;
    firstname = item->text(0);//filename
    QByteArray ha=firstname.toLocal8Bit();
    char *name=ha.data();
    name[strlen(name)-1]=0;
    path1=item->text(1);
   QByteArray da=path1.toLocal8Bit();
   char *mm=da.data();
   mm[strlen(mm)-1]=0;
    memset(chosed_path,'\0',sizeof(chosed_path));
   strcat(chosed_path,mm);

   char cmd[1024];
   memset(cmd,'\0',sizeof(cmd));
   strcat(cmd,"/opt/ass/bin/adb shell ls ");
   strcat(cmd,chosed_path);
   FILE *fp;
   char path_1[2]="/";
   strcat(cmd,path_1);
   fp=popen(cmd,"r");
   char buf[1024];

   while(!feof(fp))
   {

       char path_2[1024];
       memset(buf,'\0',1024);
       memset(path_2,'\0',1024);
      if(fgets(buf,1024,fp)==NULL)
          break;
      buf[strlen(buf)-1]=0;
      strcpy(path_2,chosed_path);
      strcat(path_2,path_1);
      int len1=strlen(path_2);
      //显示文本处理

     char *check="opendir failed, Permission denied";

     if(strncmp(buf,check,strlen(check))==0)//打开无权限
     {
        //  QMessageBox::warning(this,"Attention","Opendir failed, Permission denied");
     }
     else{
        if(strncmp(path_2,buf,len1-1)==0)//文本文件
        {
               // QMessageBox::warning(this,"Attention","This is a file,not dir");
                memset(get_file_path,'\0',sizeof(get_file_path));
                strcat(get_file_path,chosed_path);
          }
          else {
            memset(send_path,'\0',sizeof(send_path));
            strcat(send_path,chosed_path);

              }
     }

   }
   pclose(fp);

    send_file_exec_count=0;
   if(strncmp(send_path,chosed_path,sizeof(chosed_path))==0)
   {
       send_file_exec_count++;
       connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(pushButton_9_choose()));
   }

   if(strncmp(get_file_path,chosed_path,sizeof(chosed_path))==0)
   {
       send_file_exec_count++;
       connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(get_file_slot()));
   }
}

//get file
void MainWindow::get_file_slot()
{
    if(send_file_exec_count==1)
    {
         send_file_exec_count++;
         QString cmd="/opt/ass/bin/adb pull ";
         QString path=QFileDialog::getExistingDirectory(this,tr("choose dir"));
         if(path.length() == 0) {

                       QMessageBox::information(this, tr("Path"), tr("You didn't choose any directory."));
          } else {

                       QMessageBox::information(this, tr("Path"), tr("You selected ") + path);
                       cmd += chosed_path;
                       QString str=QString(QLatin1String(send_path));
                       cmd += " " + path + "/";
                       qDebug()<<"send: "<<cmd;
                       QByteArray p=cmd.toLocal8Bit();
                       char *sys_cmd=p.data();
                       system(sys_cmd);
                       QMessageBox::information(this, tr("Hello"), tr("get file success ") + path);
                    }
         }


}
//发送文件
void MainWindow::pushButton_9_choose()
{

    if(send_file_exec_count==1)
    {
        send_file_exec_count++;
    QString cmd="/opt/ass/bin/adb push ";

    //路径
    QString path = QFileDialog::getOpenFileName(this,tr("open file"));

    if(path.length() == 0) {

                  QMessageBox::information(this, tr("Path"), tr("You didn't select any files."));
     } else {

                  QMessageBox::information(this, tr("Path"), tr("You selected ") + path);
                  cmd += path;
                  QString str=QString(QLatin1String(send_path));
                  cmd += " " + str + "/";
                  qDebug()<<"send: "<<cmd;
                  QByteArray p=cmd.toLocal8Bit();
                  char *sys_cmd=p.data();
                  system(sys_cmd);
                  QMessageBox::information(this, tr("Hello"), tr("send over ") + path);
               }
    }

}

CoolButton::CoolButton(QWidget *parent)
    :QToolButton(parent)
{
    setDown(false);
    setFocusPolicy(Qt::NoFocus);
}

CoolButton::~CoolButton()
{

}

void CoolButton::enterEvent(QEvent *event)
{
    if(isEnabled() && !m_HoverIcon.isNull() )
        setIcon(m_HoverIcon);
}

void CoolButton::leaveEvent(QEvent *event)
{
    if(isEnabled())
        setIcon(m_NormalIcon);
}

void CoolButton::mousePressEvent(QMouseEvent *event)
{
    if( isEnabled() && !m_PressedIcon.isNull() )
        setIcon(m_PressedIcon);
    QToolButton::mousePressEvent(event);
}

void CoolButton::mouseReleaseEvent(QMouseEvent *event)
{
    if( isEnabled() && !m_HoverIcon.isNull() )
        setIcon(m_HoverIcon);
    QToolButton::mouseReleaseEvent(event);
}

void CoolButton::SetButtonIcons(const QIcon &normal, const QIcon &hover, const QIcon &pressed)
{
    m_NormalIcon = normal;
    m_HoverIcon = hover;
    m_PressedIcon = pressed;
    setIcon(m_NormalIcon);
}
//删除联系人
void MainWindow::pushButton_6_del()
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("\/tmp\/ass\/contacts.db3");
    bool success = db.open();

    contact_model = new QSqlTableModel(this);


   QSqlQuery  query_contact(db);
    QString Contactid_search;
    int Contactid_result;

    QByteArray tmp1 = contact_db_title.toLocal8Bit();

    QString contactid = QString::number(contact_db_index);

    Contactid_search = "select personid from contactdb limit  "+contactid+",1";
    qDebug()<<Contactid_search;
    query_contact.exec(Contactid_search);
    if(query_contact.first())
          Contactid_result =  query_contact.value(0).toInt();
   qDebug()<<Contactid_result;

    char *c_str1 = tmp1.data();

    qDebug()<<"name"<<tmp1;


     int ok = QMessageBox::warning(this, tr(" delete this contact people"),tr("are you sure"),QMessageBox::Yes,QMessageBox::No);

     if(ok == QMessageBox::No)
     {
     }
    else
    {
         contact_model->removeRow(contact_db_index);
         memset(&change_contact,'\0',sizeof(contact_cmd_t));
         if(Contactid_result != 0)
                 change_contact.id  = Contactid_result;
         set_contact_cmd(&change_contact);
         post_refresh(CMD_DELETE_CONTACT);      
         QMessageBox::about(this,tr(""),tr(" 删除成功"));
         on_getlist_clicked();
        }
     db.close();
}

//保存修改
void MainWindow::on_pushButton_clicked()
{
    QString namevalue=ui->lineEdit->text();
    QString numbervalue=ui->lineEdit_3->text();

    qDebug()<<namevalue;
    qDebug()<<numbervalue;

  //  contact_db_title=namevalue;
   // contact_db_text=numbervalue;

    QByteArray tmp1 = namevalue.toLocal8Bit();
    QByteArray tmp2 = numbervalue.toLatin1();

    char *namestr1 = tmp1.data();
    char *numberstr2 = tmp2.data();
    memset(&change_contact,'\0',sizeof(contact_cmd_t));

    strcpy(change_contact.name,namestr1);
    strcpy(change_contact.num,numberstr2);
    qDebug()<<change_contact.name;
    qDebug()<<change_contact.num;

   set_contact_cmd(&change_contact);

   post_refresh(CMD_INSERT_CONTACT);

   ui->label_2->hide();
   ui->label_5->hide();
   ui->label_4->hide();
   ui->lineEdit->hide();
   ui->lineEdit_3->hide();
  QMessageBox::about(this,tr(""),tr("添加成功"));

  on_getlist_clicked();
}

//添加联系人
void MainWindow::on_pushButton_3_clicked()
{
    ui->lineEdit->clear();
    ui->lineEdit_3->clear();
    ui->pushButton->setEnabled(true);
    ui->label_5->setText("添加联系人");
    ui->label_5->setStyleSheet("font: 20pt \"黑体\";");
    ui->label_2->setText("姓名：");
    ui->label_2->setStyleSheet("font: 20pt \"黑体\";");
    ui->label_4->setText("号码：");
    ui->label_4->setStyleSheet("font: 20pt \"黑体\";");
    ui->lineEdit->setStyleSheet("font: 26pt \"黑体\";");
    ui->lineEdit->setStyleSheet("border :1px solid rgb(255,255,255);background :rgb(110,108,108)");

    ui->lineEdit_3->setStyleSheet("font: 26pt \"黑体\";");
    ui->lineEdit_3->setStyleSheet("border :1px solid rgb(255,255,255);background :rgb(110,108,108)");


     ui->gridLayout_2->addWidget(ui->label_5,0,1,1,2);
     ui->gridLayout_2->addWidget(ui->label_2,1,0,1,1);
     ui->gridLayout_2->addWidget(ui->lineEdit,1,1,1,1);
     ui->gridLayout_2->addWidget(ui->label_4,2,0,1,1);
     ui->gridLayout_2->addWidget(ui->lineEdit_3,2,1,1,1);
     this->setLayout(ui->gridLayout_2);
     ui->gridLayout_2->setRowStretch(0,1);
     ui->gridLayout_2->setRowStretch(1,3);
     ui->gridLayout_2->setRowStretch(2,5);
     ui->gridLayout_2->setRowStretch(3,15);

     ui->gridLayout_2->setColumnStretch(0,1);
     ui->gridLayout_2->setColumnStretch(1,5);
     ui->gridLayout_2->setColumnStretch(2,1);
     ui->label_5->show();
     ui->label_2->show();
     ui->label_4->show();
     ui->lineEdit->show();
     ui->lineEdit_3->show();


}
//连接模块初始化
void MainWindow::connect_init()
{
    qDebug()<<"connect init is runninng";
    QProcess* connect = new QProcess;
    QString connect_str, daemon_str;
    connect->start("/opt/ass/bin/adb shell getprop ro.product.model");
    connect->waitForFinished();
    daemon_str = connect->readAll();
    connect_str = connect->readAllStandardError();
      qDebug()<<connect_str;
    if(connect_str.contains("error"))
    {
          QFont f;
          f.setPointSize(36);
          ui->textBrowser->setText("未发现设备");
          ui->textBrowser->setFont(f);
          ui->textBrowser->setStyleSheet("color : rgb(110,108,108) ; background-image : url(:/img/pics/nobg.png) ; font-size : 36px");
          ui->textBrowser->setAlignment(Qt::AlignCenter);
     }
    else{
        ui->label_3->setStyleSheet("background-image : url(:/img/pics/load2.jpg);border-image:url(:/img/pics/nobg.png)");
        ui->pushButton_7->hide();
        QFont ft;
        ft.setPointSize(15);
        ui->textBrowser->setText(daemon_str);
        ui->textBrowser->setStyleSheet("color: rgb(110,108,108);background-image : url(:/img/pics/nobg.png) ; font-size : 30px");
        ui->textBrowser->setAlignment(Qt::AlignCenter);
        ui->textBrowser->setFont(ft);
//显示内存容量以及电量

        memory->setGeometry(402,202,25,25);
        memory->setStyleSheet("background-image : url(:/img/pics/memory.png)");
        memory->show();

        sd->setGeometry(402,282,25,25);
        sd->setStyleSheet("background-image : url(:/img/pics/sd.png)");
        sd->show();

         tx1->setGeometry(430,202,100,25);
         tx1->setStyleSheet("color : rgb(110,108,108); background-image : url(:/img/pics/nobg.png) ; border-image : url(:/img/pics/nobg.png)");
         tx1->setText("RAM: ");
         tx1->setFont(ft);
         tx1->show();

        tx2->setGeometry(430,282,100,25);
        tx2->setStyleSheet("color : rgb(110,108,108) ; background-image : url(:/img/pics/nobg.png) ; border-image : url(:/img/pics/nobg.png)");
        tx2->setText("ROM: ");
       tx2->setFont(ft);
       tx2->show();


      tx3->setGeometry(500,225,200,25);
      tx3->setStyleSheet("color : rgb(110,108,108) ; background-image : url(:/img/pics/nobg.png) ; border-image : url(:/img/pics/nobg.png)");
	//显示ram容量
    QString str_1="共:";
       double total_1;
       double used_1;
       double value_1;
       char buf1[255];
       char totalsize1[100]={0};
       char usedsize1[100]={0};
       char freesize1[10]={0};
       FILE *fp1;
       int p=1;
       fp1=popen("/opt/ass/bin/adb shell df /system","r");
       while(!feof(fp1))
       {
           if(fgets(buf1,255,fp1)==NULL)
               break;

           if(p==2)
           {
               //printf("%s",buf);
                   sscanf(buf1,"%*s%s%s%s",totalsize1,usedsize1,freesize1);
                   QString  totalsize_2;
                   totalsize_2 +=totalsize1;
                   totalsize_2=totalsize_2.left(totalsize_2.length()-1);

                   QString  usedsize_2;
                   usedsize_2 +=usedsize1;
                   usedsize_2=usedsize_2.left(usedsize_2.length()-1);
                   used_1=usedsize_2.toDouble();
                   total_1=totalsize_2.toDouble();
                   value_1=used_1/total_1;

                   bar1->setGeometry(500,202,200,21);
                   bar1->setRange(0,100);
                   bar1->setValue(value_1*100);
                   bar1->setTextVisible(false);
                   bar1->show();
                   str_1 += totalsize1;
                   str_1 += ",已用:";
                   str_1 += usedsize1 ;
                   tx3->setText(str_1);
                   tx3->setStyleSheet("color : rgb(110,108,108) ; border-image : url(:/img/pics/nobg.png); background-image : url(:/img/pics/nobg.png)");
           }
           p++;
       }
       pclose(fp1);


    //sdcard:

       tx4->setGeometry(500,305,200,25);

//显示rom容量
           QString str_2="共:";
           double total_2;
           double used_2;
           double value_2;
           char buf[255];
           char totalsize[100]={0};
           char usedsize[100]={0};
           char freesize[10]={0};
           FILE *fp;
           int i=1;
           fp=popen("/opt/ass/bin/adb shell df /data","r");
           while(!feof(fp))
           {
               if(fgets(buf,255,fp)==NULL)
                   break;

               if(i==2)
               {
                       sscanf(buf,"%*s%s%s%s",totalsize,usedsize,freesize);
                       if(strcmp(totalsize,"No")==0)
                       {

                           bar2->setGeometry(500,282,200,21);
                           bar2->setRange(0,100);
                           bar2->setTextVisible(false);
                           bar2->setValue(0);
                           str_2="未检测到SD卡";
                           bar2->show();
                           tx4->setText(str_2);
                       }else{
                       QString  totalsize_1;
                       totalsize_1 +=totalsize;
                       totalsize_1=totalsize_1.left(totalsize_1.length()-1);

                       QString  usedsize_1;
                       usedsize_1 +=usedsize;
                       usedsize_1=usedsize_1.left(usedsize_1.length()-1);
                       used_2=usedsize_1.toDouble();
                       total_2=totalsize_1.toDouble();
                       value_2=used_2/total_2;
                     //  bar2 =new QProgressBar(ui->tab);
                       bar2->setGeometry(500,282,200,21);
                       bar2->setRange(0,100);
                       bar2->setTextVisible(false);
                       bar2->setValue(value_2*100);
                       //bar1->show();
                       bar2->show();
                       str_2 += totalsize;
                       str_2 += ",已用:";
                       str_2 += usedsize ;

                       tx4->setText(str_2);
                       }
                       tx4->setStyleSheet("color : rgb(110,108,108) ; border-image : url(:/img/pics/nobg.png); background-image : url(:/img/pics/nobg.png)");
               }
               i++;
           }
           pclose(fp);

       //显示电池电量
       QString str_3="电池电量: ";
       QProcess process_3;
       process_3.start("/opt/ass/bin/adb shell cat /sys/class/power_supply/battery/capacity");
       process_3.waitForFinished();
       QByteArray qbaOutput_3 = process_3.readAllStandardOutput();
       QString qstrYouWant_3 = qbaOutput_3;
       QByteArray ba=qstrYouWant_3.toLatin1();
       char *pq;
       pq=ba.data();
       //qstrYouWant_3;
       //qstrYouWant_3[strlen(qstrYouWant_3)-1]=0;
       pq[strlen(pq)-1]=0;
       str_3 +=pq;
       str_3 += "%";
       //QProcess::execute("/opt/ass/bin/adb shell cat /sys/class/power_supply/battery/capacity");


       bat->setGeometry(400,370,25,25);
       bat->setStyleSheet("background-image : url(:/img/pics/battery.png)");
       bat->show();

       tx5->setGeometry(430,365,200,35);
       tx5->setStyleSheet("color : rgb(110,108,108) ; background-image : url(:/img/pics/nobg.png) ; border-image : url(:/img/pics/nobg.png)");
       tx5->setText(str_3);
       tx5->setFont(ft);
       tx3->show();
       tx4->show();
       tx5->show();

       ui->show_button_1->setEnabled(true);
        ui->toolButton_2->setEnabled(true);
        ui->toolButton_3->setEnabled(true);
        ui->getlist->setEnabled(true);
        ui->Btn_mmsrefresh->setEnabled(true);
        ui->Btn_sendmms->setEnabled(true);
        ui->Btn_audio->setEnabled(true);
        ui->Btn_audioBackup->setEnabled(true);
        ui->Btn_image->setEnabled(true);
        ui->Btn_imageBackup->setEnabled(true);
        ui->Btn_video->setEnabled(true);
        ui->Btn_videBackup->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_9->setEnabled(true);
        ui->Btn_appbackup->setEnabled(true);
        ui->Btn_appinstlal->setEnabled(true);
        ui->Btn_uninstall->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->Btn_back->setEnabled(true);
        ui->Btn_forward->setEnabled(true);
        ui->Btn_reload->setEnabled(true);
        //ui->pushButton_5->setEnabled(true);

}

}
//截屏保存到本地
void MainWindow::on_show_button_1_clicked()
{
    screenshotinit();
    QFile plik;
     plik.setFileName(QFileDialog::getSaveFileName(this, tr("Save File..."), "/home", tr("Png file")+" (*.png)"));
     if (plik.fileName().isEmpty())
            return;
     if (plik.open(QFile::WriteOnly))
     {
         QMatrix matrix;

         QImage image("/tmp/ass/screenshot.png");
         image = image.transformed(matrix);

         image.save(&plik, "PNG");
         plik.close();
     }
}
//截屏操作
void MainWindow::screenshotinit()
{
    QProcess::execute("/opt/ass/bin/adb shell /system/bin/screencap -p /sdcard/screenshot.png");
    QProcess::execute("/opt/ass/bin/adb pull /sdcard/screenshot.png   /tmp/ass/screenshot.png");
    QPixmap  pixmap1("/tmp/ass/screenshot.png");
    QPixmap pixmap(pixmap1.scaled(191,341, Qt::KeepAspectRatio));
    ui->label_3->setPixmap(pixmap);
    ui->label_3->setStyleSheet("background-image : url(:/img/pics/nobg.png)");

}
//刷新按钮
void MainWindow::on_toolButton_3_clicked()
{
    screenshotinit();
}
//演示按钮
void MainWindow::on_toolButton_2_clicked()
{
    Dialog_show *screen=new Dialog_show;
    screen->show();
}

//应用市场页面加载
void MainWindow::LoadWebview()
{
    urladdr = new QLineEdit;
    page =new webPage(this);

    ui->webView->setPage(page);
    connect(ui->Btn_back, SIGNAL(clicked()), ui->webView, SLOT(back()));
    connect(ui->Btn_forward, SIGNAL(clicked()), ui->webView, SLOT(forward()));
    connect(ui->Btn_reload, SIGNAL(clicked()), ui->webView, SLOT(reload()));

    connect(this->page,SIGNAL(adddownloadurl(int,QString,int,int,QString)),this,SLOT(adddownloadurl(int,QString,int,int,QString)));
    gridlayout_init();
    ui->webView->load(QUrl("http://apk.hiapk.com/"));
    ui->webView->setZoomFactor(0.80);
    ui->label_6->hide();
    ui->label_7->hide();
    ui->progressBar->hide();
}

void MainWindow::gridlayout_init()
{
    ui->gridLayout_3->setRowStretch(0,1);
    ui->gridLayout_3->setRowStretch(1,1);
    ui->gridLayout_3->setColumnStretch(0,2);
    ui->gridLayout_3->setColumnStretch(1,5);
    ui->gridLayout_3->setColumnStretch(2,3);
    ui->gridLayout_3->addWidget(ui->label_6,0,0,1,1);
    ui->gridLayout_3->addWidget(ui->label_7,0,2,1,1);
    //ui->gridLayout_3->addWidget(ui->label_7,0,2,1,1,);
    ui->gridLayout_3->addWidget(ui->progressBar,0,1,1,1);
}

void MainWindow::adddownloadurl(int count, QString speed, int percent, int stat, QString filename)
{
    ui->label_6->show();
    ui->label_7->show();
    ui->progressBar->show();
    ui->label_6->setText(filename);
    ui->label_7->setText(speed);
    ui->progressBar->setValue(percent);

    if(percent == 100)
    {
        ui->label_6->hide();
        ui->label_7->hide();
        ui->progressBar->hide();
    }
}
void MainWindow::startRequest(QUrl url)
{

    reply = manager->get(QNetworkRequest(url));
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));
    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));

}
void MainWindow::httpReadyRead()
{
    if(file)file->write(reply->readAll());
}
void MainWindow::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    //ui->progressBar->setMaximum(totalBytes);
   // ui->progressBar->setValue(bytesRead);
}
void MainWindow::httpFinished()
{

  // ui->progressBar->hide();
    file->flush();
    file->close();
    reply->deleteLater();
    reply=0;
    delete file;
    file=0;
}

void MainWindow::on_webView_linkClicked(const QUrl &arg1)
{
   // QWebView *webView=new QWebView;
    //connect(webView->page(), SIGNAL(downloadRequested(const QNetworkRequest&)), this, SLOT(download(co nst QNetworkRequest&)));
}
//开始连接按钮
void MainWindow::on_pushButton_7_clicked()
{
    post_daemon();
    connect_init();
}


void MainWindow::setui_clean()
{
   memset(&uimmsinfo,'\0',sizeof(message_pack_t));
   memset(&uiaudioinfo,'\0',sizeof(sizeof(audio_pack_t)));
   memset(&uivideoinfo,'\0',sizeof(video_pack_t));
   memset(&uiimageinfo,'\0',sizeof(video_pack_t));
}
void MainWindow::display(int a)//线程槽处理函数
{
    emit device_offline();
    Disconnect_Thread->stop();
    setui_clean();
    set_end_flag(1);
    QProcess *Source_clear  = new QProcess;
      if(a==2)
      {
         QFont f;
         f.setPointSize(36);
         ui->textBrowser->setText("未发现设备");
         ui->textBrowser->setFont(f);
         ui->textBrowser->setStyleSheet("color :rgb(110,108,108l) ; background-image : url(:/img/pics/nobg.png) ; font-size : 36px");
         ui->textBrowser->setAlignment(Qt::AlignCenter);

         FILE *fp;
        char buf[50];
         fp=popen("ls /tmp/ass/","r");
          while(!feof(fp))
           {
                 memset(buf,'\0',50);
                  if(fgets(buf,50,fp)==NULL)
                     break;

                   QString Clear_output = QString(buf);
                   Clear_output = "rm  -r /tmp/ass/" + Clear_output;
                   qDebug()<<Clear_output;

                   Source_clear->start(Clear_output);
                   Source_clear->waitForFinished();
             }
           pclose(fp);
        QMessageBox::warning(this,"未连接","你的设备已离线");
        ui->pushButton_7->show();
        tx1->hide();
        tx2->hide();
        tx3->hide();
        tx4->hide();
        tx5->hide();
        bar1->hide();
        bar2->hide();
        ui->label_3->setStyleSheet("image:url(:/img/pics/load1.jpg)");
        memory->setStyleSheet("background-image:url(:/img/pics/nobg.png)");
        sd->setStyleSheet("background-image:url(:/img/pics/nobg.png)");
        bat->setStyleSheet("background-image:url(:/img/pics/nobg.png)");
        //UiToolInit();
        ui->label_3->clear();
        ui->listWidget->clear();
        ui->Lstvimagelist->clear();

        setui_apps();
        LoadImage();
        LoadVideo();
        setui_mmsinfo();
        Mms_table->hide();
        ui->music_widget->clear();
         LoadMms();
         LoadAudio();

       ui->mms_widget->clearContents();

              showload_app->close();
             showload_audio->close();
             showload_contacts->close();
             showload_image->close();
             showload_mms->close();
             showload_video->close();
     on_toolButton_4_clicked();


       ui->show_button_1->setEnabled(false);
       ui->toolButton_2->setEnabled(false);
       ui->toolButton_3->setEnabled(false);
        ui->getlist->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
       ui->pushButton_6->setEnabled(false);
        ui->Btn_mmsrefresh->setEnabled(false);
        ui->Btn_sendmms->setEnabled(false);
        ui->Btn_audio->setEnabled(false);
        ui->Btn_audioBackup->setEnabled(false);
        ui->Btn_image->setEnabled(false);
         ui->Btn_imageBackup->setEnabled(false);
          ui->Btn_video->setEnabled(false);
          ui->Btn_videBackup->setEnabled(false);
          ui->pushButton_2->setEnabled(false);
          ui->pushButton_9->setEnabled(false);
          ui->Btn_appbackup->setEnabled(false);
          ui->Btn_appinstlal->setEnabled(false);
          ui->Btn_uninstall->setEnabled(false);
          ui->pushButton_4->setEnabled(false);
          ui->Btn_back->setEnabled(false);
          ui->Btn_forward->setEnabled(false);
          ui->Btn_reload->setEnabled(false);
          ui->tableView->setStyleSheet("background-image:url(:/img/pics/load.png)");
          ui->tableView->show();
          ui->mms_widget->setStyleSheet("background-image:url(:/img/pics/load.png)");
        //  ui->music_widget->setStyleSheet("image:url(:/img/pics/load.png)");
          ui->Lstvimagelist->setStyleSheet("background-image:url(:/img/pics/load.png)");
          ui->Video_list->setStyleSheet("background-image:url(:/img/pics/load.png)");
          ui->table_widget_third->setStyleSheet("background-image:url(:/img/pics/load.png)");
          //ui->pushButton_5->setEnabled(false);

        }
}

void MainWindow::Clear_all()
{
    setui_clean();
   QProcess *Source_clear  = new QProcess;
    FILE *fp;
    char buf[50];
    fp=popen("ls /tmp/ass/","r");
    while(!feof(fp))
   {
         memset(buf,'\0',50);
          if(fgets(buf,50,fp)==NULL)
             break;

          QString Clear_output = QString(buf);
          Clear_output = "rm  -r /tmp/ass/" + Clear_output;
         qDebug()<<Clear_output;

        Source_clear->start(Clear_output);
         Source_clear->waitForFinished();
        }
      pclose(fp);
}
void MainWindow::on_Btn_mmsrefresh_clicked()
{
    ui->Btn_mmsrefresh->setStyleSheet("QPushButton{border-image: url(:/img/pics/sx2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_sx2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_sx2.png);}");
    ui->mms_widget->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
}
//应用程序关闭按钮槽
void MainWindow::on_toolButton_10_clicked()
{
    FILE *fp;
      char buf[50];
            fp=popen("ls /tmp/ass/","r");
            while(!feof(fp))
            {
                memset(buf,'\0',50);
                if(fgets(buf,50,fp)==NULL)
                    break;
              // qDebug()<<buf;
              if( strncmp(buf,"screenshot.png",14)==0 )
              {
                  QProcess::execute("rm /tmp/ass/screenshot.png");
              }
            }
            pclose(fp);
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->table_widget_third->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    post_refresh(CMD_APPINFO);

    QMovie *mov = new QMovie(":/img/pics/1.gif");
            showload_app->setMovie(mov);
            showload_app->setGeometry(360,160,300,300);
            showload_app->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
            showload_app->show();
             mov->start();

}

void MainWindow::on_Btn_audio_clicked()
{
    ui->music_widget->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    ui->Btn_audio->setStyleSheet("QPushButton{border-image: url(:/img/pics/sx2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_sx2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_sx2.png);}");
}

void MainWindow::on_Btn_image_clicked()
{
    ui->Lstvimagelist->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    ui->Btn_image->setStyleSheet("QPushButton{border-image: url(:/img/pics/sx2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_sx2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_sx2.png);}");
}

void MainWindow::on_Btn_video_clicked()
{
    ui->Video_list->setStyleSheet("background-image : url(:/img/pics/nobg.png)");
    ui->Btn_video->setStyleSheet("QPushButton{border-image: url(:/img/pics/sx2.png);background-image : url(:/img/pics/nobg.png)}"
                                      "QPushButton:hover{border-image: url(:/img/pics/h_sx2.png);}"
                                      "QPushButton:pressed{border-image: url(:/img/pics/h_sx2.png);}");
}

void MainWindow::on_toolButton_11_clicked()
{
    this->showMinimized();
    this->setWindowState(Qt::WindowNoState);
}
