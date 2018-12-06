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

HEADERS  += ffplayer.h \
    ffplayer_video.h \
    ffplayer_audio.h \
    ffplayer_general.h \
    ffplayer_sdl.h

FORMS    += ffplayer.ui


#----------------- FFmpeg -----------------
INCLUDEPATH += ../FFmpeg/include

LIBS += ../FFmpeg/lib/libavcodec.dll.a\
	../FFmpeg/lib/libavdevice.dll.a\
	../FFmpeg/lib/libavfilter.dll.a\
	../FFmpeg/lib/libavformat.dll.a\
	../FFmpeg/lib/libavutil.dll.a\
	../FFmpeg/lib/libswresample.dll.a\
	../FFmpeg/lib/libswscale.dll.a\
	../FFmpeg/lib/libpostproc.dll.a




#----------------- SDL -----------------
INCLUDEPATH += ../SDL2-2.0.9/include

LIBS += -L../SDL2-2.0.9/lib/x86 -lSDL2
LIBS += -L../SDL2-2.0.9/lib/x86 -lSDL2main
LIBS += -L../SDL2-2.0.9/lib/x86 -lSDL2test


