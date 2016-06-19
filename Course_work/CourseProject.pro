#-------------------------------------------------
#
# Project created by QtCreator 2016-06-08T13:13:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CourseProject
TEMPLATE = app


SOURCES += main.cpp\
        odcrypt.cpp \
    thread.cpp

HEADERS  += odcrypt.h \
    thread.h

FORMS    += odcrypt.ui

LIBS += -lcryptopp
