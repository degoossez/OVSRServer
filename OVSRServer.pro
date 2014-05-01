#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T10:26:53
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OVSRServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    databasemanager.cpp \
    logindialog.cpp

HEADERS  += mainwindow.h \
    server.h \
    databasemanager.h \
    logindialog.h

FORMS    += mainwindow.ui
