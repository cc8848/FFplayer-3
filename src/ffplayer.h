#ifndef FFPLAYER_H
#define FFPLAYER_H

#include <QMainWindow>
#include <QDebug>

#define __STDC_CONSTANT_MACROS

using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavformat/version.h>
#include <libavutil/time.h>
#include <libavutil/mathematics.h>

/*** FFmpeg log System
 * av_log_set_level(AV_LOG_DEBUG);
 * av_log(NULL, AV_LOG_INFO, "...%s\n", op);
 **/
#include <libavutil/log.h>

/***
 *
 *
 **/

}

namespace Ui {
class FFPlayer;
}

class FFPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit FFPlayer(QWidget *parent = 0);
    ~FFPlayer();

private:
    Ui::FFPlayer *ui;
};

#endif // FFPLAYER_H
