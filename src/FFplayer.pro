#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T15:56:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFplayer
TEMPLATE = app


SOURCES += main.cpp\
        ffplayer.cpp

HEADERS  += ffplayer.h

FORMS    += ffplayer.ui


#----------------- FFmpeg -----------------
INCLUDEPATH += F:/FFplayer/FFmpeg/include

LIBS += F:/FFplayer/FFmpeg/lib/libavcodec.dll.a\
        F:/FFplayer/FFmpeg/lib/libavdevice.dll.a\
        F:/FFplayer/FFmpeg/lib/libavfilter.dll.a\
        F:/FFplayer/FFmpeg/lib/libavformat.dll.a\
        F:/FFplayer/FFmpeg/lib/libavutil.dll.a\
        F:/FFplayer/FFmpeg/lib/libswresample.dll.a\
        F:/FFplayer/FFmpeg/lib/libswscale.dll.a\
        F:/FFplayer/FFmpeg/lib/libpostproc.dll.a




#----------------- SDL -----------------
INCLUDEPATH += F:/FFplayer/SDL2-2.0.9/include

LIBS += -LF:/FFplayer/SDL2-2.0.9/lib/x86 -lSDL2
LIBS += -LF:/FFplayer/SDL2-2.0.9/lib/x86 -lSDL2main
LIBS += -LF:/FFplayer/SDL2-2.0.9/lib/x86 -lSDL2test

#第一种方法：格式为:  LIBS += -L+相对路径+空格+-l+库文件的名字（不加.lib）
#第二种方法：格式为:  LIBS += -L+绝对路径+空格+-l+库文件的名字（不加.lib）
#第三种方法：格式为:  LIBS += -L+相对路径++库文件的名字（加.lib）
#第四种方法：
    #LIBS += -L../lib/x86
    #LIBS += SDL2.lib


