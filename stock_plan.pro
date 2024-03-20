#-------------------------------------------------
#
# Project created by QtCreator 2023-07-06T21:17:32
#
#-------------------------------------------------

QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stock_plan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        clientmainwindow.cpp \
    getstocklist.cpp \
    stockpolicy.cpp

HEADERS += \
        clientmainwindow.h \
    getstocklist.h \
    stockpolicy.h

FORMS += \
        clientmainwindow.ui \
    getstocklist.ui \
    stockpolicy.ui
