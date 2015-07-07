#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T12:01:20
#
#-------------------------------------------------

QT       += core gui opengl svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IslamicStarPatterns
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    GLContainer.cpp \
    GLWidget.cpp

HEADERS  += mainwindow.h \
    ALine.h \
    AVector.h \
    GLContainer.h \
    GLWidget.h \
    VertexData.h \
    Tiling.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -frounding-math -O3

QMAKE_CXXFLAGS += -std=gnu++1y
