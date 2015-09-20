#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T12:54:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TARGET = GLCube
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        cube.cpp

HEADERS  += cube.h

RESOURCES += \
    resources.qrc
