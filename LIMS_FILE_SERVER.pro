#-------------------------------------------------
#
# Project created by QtCreator 2017-05-02T17:26:47
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LIMS_FILE_SERVER
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    clientmanagement.cpp \
    filemanagement.cpp \
    settingdialog.cpp \
    client.cpp \
    filesendtask.cpp \
    signalingparsemodule.cpp \
    log.cpp \
    clientsinfomodel.cpp


HEADERS  += mainwindow.h \
    clientmanagement.h \
    filemanagement.h \
    settingdialog.h \
    client.h \
    filesendtask.h \
    signalingparsemodule.h \
    config.h \
    log.h \
    clientsinfomodel.h



FORMS    += mainwindow.ui \
    settingdialog.ui
