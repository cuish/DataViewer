#-------------------------------------------------
#
# Project created by QtCreator 2014-09-01T00:56:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    graphview.cpp \
    tablemodel.cpp

HEADERS  += mainwindow.h \
    graphview.h \
    tablemodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    graphview.qrc
