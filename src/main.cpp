#include <QApplication>
#include <stdio.h>
#include <QDir>

#include <SDL.h>
#include "ffplayer.h"
#include "ffplayer_general.h"
#include "ffplayer_audio.h"
#include "ffplayer_video.h"
#include "ffplayer_sdl.h"
#undef main



int video_decode(char *filepath);
int fetch_audio(char *srcPath, char *dstPath);
int sdl2_test(char *filepath);
int fetch_h264(char *srcPath, char *dstPath);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FFPlayer w;

    //video_decode("../testData/Titanic.ts");
    //sdl2_test("../testData/Hello_World.bmp");
    fetch_audio("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.aac");
    //fetch_h264("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.h264");

    return 0;    //a.exec()
}







