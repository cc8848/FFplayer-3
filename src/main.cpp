#include <QApplication>
#include <stdio.h>
#include <QDir>

#include <SDL.h>
#include "ffplayer.h"
#include "ffplayer_audio.h"
#include "ffplayer_video.h"
#include "ffplayer_sdl.h"
#undef main



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    av_log_set_level(AV_LOG_INFO);   // AV_LOG_ERROR AV_LOG_WARNING AV_LOG_INFO AV_LOG_DEBUG

    //video_decode("../testData/Titanic.ts", "../testData/Titanic.h264", "../testData/Titanic.YUV", "../testData/Titanic.txt");
    //sdl2_test("../testData/Hello_World.bmp");
    //fetch_audio_HE("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.aac");
    //fetch_audio_LC("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.aac");
    //fetch_h264("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.h264");

    return 0;    //a.exec()
}







