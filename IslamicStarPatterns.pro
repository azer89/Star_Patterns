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
    GLWidget.cpp \
    tinyxml2.cpp \
    PatternGenerator.cpp \
    SystemParams.cpp \
    Triangulator.cpp

HEADERS  += mainwindow.h \
    ALine.h \
    AVector.h \
    GLContainer.h \
    GLWidget.h \
    VertexData.h \
    tinyxml2.h \
    PatternGenerator.h \
    TilingData.h \
    SystemParams.h \
    RibbonSegment.h \
    Triangulator.h

FORMS    += mainwindow.ui

INCLUDEPATH +=   /usr/include/
LIBS        += -L/usr/include/
LIBS        += -lCGAL
LIBS        += -lgmp
LIBS        += -lmpfr

QMAKE_CXXFLAGS += -frounding-math -O3

QMAKE_CXXFLAGS += -std=gnu++1y

OTHER_FILES += \
    archimedeans.xml \
    hanbury.xml
