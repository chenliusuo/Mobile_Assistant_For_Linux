#-------------------------------------------------
#
# Project created by QtCreator 2014-12-26T16:03:51
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += webkitwidgets
QT       += network
QT       += webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = manager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    thread_1.cpp \
    dialog_show.cpp \
    dialog_1.cpp \
    dialog.cpp \
    backthread.cpp \
    recvthread.cpp \
    webpage.cpp \
    daemon.cpp \
    connectionthread.cpp \
    mms_dialog.cpp \
    thread_2.cpp \
    mmsthread.cpp \
    customwidgetitem.cpp \
    install_app.cpp \
    disconnect_thread.cpp

HEADERS  += mainwindow.h \
    ToolButton.h \
    thread_1.h \
    dialog_show.h \
    dialog_1.h \
    dialog.h \
    backthread.h \
    assevent.h \
    recvthread.h \
    webpage.h \
    daemon.h \
    connectionthread.h \
    mms_dialog.h \
    thread_2.h \
    mmsthread.h \
    customwidgetitem.h \
    install_app.h \
    disconnect_thread.h

FORMS    += mainwindow.ui \
    dialog_show.ui \
    dialog_1.ui \
    dialog.ui \
    MMS_dialog.ui

#LIBS += -L . -lsockClient

LIBS += -L . -lass
LIBS += -L . -lserver

RESOURCES += \
    img.qrc
	#target
	target.files += manager
	target.path = /usr/bin
	#icons
	icons.files += build/pics
	icons.path = /usr/share/pixmaps/

	#desktop
	desktop.files += build/manager.desktop
	desktop.path = /usr/share/applications/

	INSTALLS += target icons desktop
