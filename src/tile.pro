#-------------------------------------------------
#
# Project created by QtCreator 2012-04-29T22:23:49
#
#-------------------------------------------------

QT += core gui

TARGET = cheetah-texture-packer

QT_VERSION=$$[QT_VERSION]

contains(QT_VERSION, "^5.*") {
  QT += widgets
} else {
}

TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
	view.cpp \
	imagepacker.cpp \
	imagecrop.cpp \
	imagesort.cpp \
	maxrects.cpp \
    atlaspage.cpp \
    atlasbuilder.cpp \
    atlasimporter.cpp \
    commandline.cpp

HEADERS  += mainwindow.h \
	view.h \
	imagepacker.h \
	maxrects.h \
    atlaspage.h \
    atlasbuilder.h \
    atlasimporter.h \
    commandline.h
FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -Wextra -Werror

TRANSLATIONS += tile_ru.ts
