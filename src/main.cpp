#include <QApplication>
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
#if 0
    //video_decode("../testData/Titanic.ts", "../testData/Titanic.h264", "../testData/Titanic.YUV", "../testData/Titanic.txt");
    sdl2_showColor("SDL2_showColor", 200, 200, 400, 400,
                   255, 0, 0, 255, SDL_WINDOW_SHOWN);
    sdl2_showPic("../testData/Hello_World.bmp");
    sdl2_texture("SDL2_showColor", 500, 500, 500, 500,
                   255, 0, 0, 255, SDL_WINDOW_SHOWN);
    //sdl2_YUVplayer(650, 370, 640, 360);

    fetch_audio_HE("../testData/mediaFormat/aac_chenrui.aac", "../testData/mediaFormat/aac_chenrui_HE.aac");
    fetch_audio_LC("../testData/mediaFormat/aac_chenrui.aac", "../testData/mediaFormat/aac_chenrui_LC.aac");
    encode_audio("../testData/encode_audio.aac");

#endif

    //fetch_h264("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.h264");

    //remuxing("../testData/killer.mp4", "../testData/remuxing.flv");

    decode_audio("../testData/killer.mp4", "../testData/decode_audio.aac");
    //encode_video("../testData/encode_video.h264", "libx264");
    //decode_video("../testData/killer.mp4", "../testData/decode_video/pic");

    /* cut_video(double from_seconds, double end_seconds, const char* in_filename, const char* out_filename)
     *  程序崩溃。
     *  cut_video(10, 20, "../testData/Forrest_Gump_IMAX.mp4", "../testData/cut_video.mp4");
     */

    return 0;    //a.exec()
}
