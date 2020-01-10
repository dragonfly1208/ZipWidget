#-------------------------------------------------
#
# Project created by QtCreator 2014-07-05T23:56:22
#
#-------------------------------------------------

QT       += core gui
QT += widgets
QT += winextras
QT_PRIVATE += widgets-private
CONFIG += c++11 #debug
#LIBS += -L$$PWD/QuaZip/ -lquazip

DESTDIR = ..\bin

#QMAKE_PRE_LINK = copy QuaZip\QuaZip.dll ..\bin\
#QMAKE_PRE_LINK = copy ZipModel.zip ..\bin\

TARGET = ZipModel
TEMPLATE = app
DEFINES += QUAZIP_STATIC
SOURCES += main.cpp\
    ZipFileInfo.cpp \
    ZipModel.cpp\
    QuaZip/JlCompress.cpp \
    QuaZip/qioapi.cpp \
    QuaZip/quaadler32.cpp \
    QuaZip/quacrc32.cpp \
    QuaZip/quagzipfile.cpp \
    QuaZip/quaziodevice.cpp \
    QuaZip/quazip.cpp \
    QuaZip/quazipdir.cpp \
    QuaZip/quazipfile.cpp \
    QuaZip/quazipfileinfo.cpp \
    QuaZip/quazipnewinfo.cpp \
    QuaZip/unzip.c \
    QuaZip/zip.c \
    filesystemmodel.cpp \
    filesystemview.cpp \
    utils.cpp \
    browserwidget.cpp \
    zipwidget.cpp \
    mainwindow.cpp \
    filedialogex.cpp

HEADERS  += \
    ZipFileInfo.h \
    ZipModel.h \
    QuaZip/crypt.h \
    QuaZip/ioapi.h \
    QuaZip/JlCompress.h \
    QuaZip/quaadler32.h \
    QuaZip/quachecksum32.h \
    QuaZip/quacrc32.h \
    QuaZip/quagzipfile.h \
    QuaZip/quaziodevice.h \
    QuaZip/quazip.h \
    QuaZip/quazip_global.h \
    QuaZip/quazipdir.h \
    QuaZip/quazipfile.h \
    QuaZip/quazipfileinfo.h \
    QuaZip/quazipnewinfo.h \
    QuaZip/unzip.h \
    QuaZip/zip.h \
    filesystemmodel.h \
    filesystemview.h \
    utils.h \
    browserwidget.h \
    zipwidget.h \
    mainwindow.h \
    filedialogex.h

RESOURCES += res.qrc

INCLUDEPATH += $$PWD/QuaZip/
INCLUDEPATH += $$PWD/zlib/
win32-g++{
#    LIBS += -L$$PWD/QuaZip -lquazip
    LIBS += -L$$PWD/libs -lzlib
#INCLUDEPATH += D:/ProgramFiles/Qt5.9.8/Tools/mingw530_32/i686-w64-mingw32/include/
}

FORMS += \
    browserWidget.ui \
    mainwindow.ui
