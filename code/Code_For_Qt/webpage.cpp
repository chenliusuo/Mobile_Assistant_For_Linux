#include "webpage.h"
#include <QCoreApplication>
#include <QStringList>
#include <stdio.h>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>
#include <QFileDialog>
#include <QHeaderView>
#include <QFileIconProvider>

#include <QtCore/QDebug>

#include <QtWebKit/QWebSettings>
#include <QMessageBox>
#include <QLabel>
webPage::webPage(QWidget *parent) :
    QWebPage(parent)
{
    connect(this, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(handleUnsupportedContent(QNetworkReply *)));
    this->setForwardUnsupportedContent(true);
    Downloadthradinit();
}


bool webPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type){

    if(type==0){
        if(frame!=mainFrame()){
            qDebug() << "openlink"  << request.url();
            emit openLink(request.url());
        }else{
            qDebug() << "loadLink"  << request.url();
            emit loadLink(request.url());
        }

    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

void webPage::Downloadthradinit()
{
    //	connect(this, SIGNAL(adddownloadurl(int,double,int,int,QString)), downloadthread , SLOT(adddownloadurl(int,double,int,int,QString)));
}


void webPage::handleUnsupportedContent(QNetworkReply *reply)
{
    //	downloadcount ++;
    if (!reply || reply->url().isEmpty())
        return;
    QVariant header = reply->header(QNetworkRequest::ContentLengthHeader);
    bool ok;
    int size = header.toInt(&ok);
    if (ok && size == 0)
        return;


    qDebug() << "webPage::handleUnsupportedContent downurllist.append";
    downurllist.append(reply->url().toString());
    if(downloadsign)
    {
        //		qDebug() << downurllist.size();
        downloadsign = 0;
        startNextDownload(QUrl::fromEncoded(downurllist.takeFirst().toLocal8Bit()));
    }
}


QString webPage::saveFileName(const QUrl &url)
{
    QString path = url.path();
    basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    savefile = savedir  + basename;

    if (QFile::exists(savefile)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '/tmp/ass';
        while (QFile::exists(savedir + basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }else{

    }

    return savedir + basename;
   qDebug()<<savedir+basename;
}


void webPage::startNextDownload(QUrl url)
{

    qDebug() << "url" <<url ;

    filename = saveFileName(url);
    filename= "/tmp/ass/" + filename;

    qDebug() << "filename" << filename;
    output.setFileName(filename);

    if (!output.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Problem opening save file '%s' for download '%s': %s\n",
                qPrintable(filename), url.toEncoded().constData(),
                qPrintable(output.errorString()));

        return;                 // skip this download
    }
    downcount ++;
    emit addstartdownload(downcount,speed,percent,stat,basename);

    QNetworkRequest request(url);
    manager[downcount] = new QNetworkAccessManager;

    currentDownload[downcount] = manager[downcount]->get(request);

    connect(currentDownload[downcount], SIGNAL(readyRead()),
            SLOT(downloadReadyRead()));
    connect(currentDownload[downcount], SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));
    connect(currentDownload[downcount], SIGNAL(finished()),
            SLOT(downloadFinished()));

    printf("Downloading %s...\n", url.toEncoded().constData());
    downloadTime.start();
}

void webPage::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{

    // calculate the download speed
    speed = bytesReceived * 1000.0 / downloadTime.elapsed();
    if (downloadTime.elapsed() - beforetime < 1000)
    {
        return;
    }

    beforetime = downloadTime.elapsed();
    qDebug() << "download time: " <<  downloadTime.elapsed()  << " beforetime " <<beforetime ;
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    ++iteration;

    if (bytesTotal> 0) {
        // we know the maximum
        // draw a progress bar
        percent = bytesReceived * 100 / bytesTotal;
    }
    //        int hashes = percent / 2;
    qDebug() << "speed: " << speed << " " << unit << "percent " << percent;
    this_speed = QString::number(speed,'.',3);
    this_speed += unit;
    emit adddownloadurl(downcount,this_speed,percent,stat,basename);
}

void webPage::downloadReadyRead()
{
    output.write(currentDownload[downcount]->readAll());
}

void webPage::downloadFinished()
{
    output.close();
    qDebug() << "downloadFinished" << "downcount = "  << downcount;
     //show_down->hide();
    this_speed=" ";
    emit adddownloadurl(downcount,this_speed,100,stat,basename);
    emit downloadurlfinished(downcount,ADB_INSTALL,0,savedir + basename);
    beforetime = 0;

    if(downurllist.size())
    {
        startNextDownload(QUrl::fromEncoded(downurllist.takeFirst().toLocal8Bit()));
        qDebug() << "download next url";
    }else {
        downloadsign = 1;
    }
    install_webapp = new QThread();
    //install
   QString install="/opt/ass/bin/adb install ";
    QProcess *process1=new QProcess;
    QString output_msg;

    install +=filename;
    qDebug()<<install;

     installapp_thread = new Install_app(install,this);
    installapp_thread->start();

    connect(installapp_thread,SIGNAL(install_finished(QString)),this,SLOT(webapp_finished(QString)));
    /*
    process1->start(install);
   // process1->waitForFinished(30000);
    output_msg = process1->readAllStandardOutput();

    if(output_msg.contains("Success"))
    {
        QMessageBox::about(NULL,"提示","应用已成功安装！");
    }
    else
    {
        QMessageBox::warning(NULL,"提示","应用安装失败！");
    }

*/
}
void webPage::webapp_finished(QString webapp_result)
{
    if(webapp_result.contains("Success"))
             QMessageBox::about(NULL, tr(" "),tr("安装成功"));
    else
            QMessageBox::about(NULL,tr(""),tr("安装失败  "));
}


