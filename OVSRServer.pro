#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T10:26:53
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OVSRServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    sslserver.cpp

HEADERS  += mainwindow.h \
    server.h

FORMS    += mainwindow.ui
