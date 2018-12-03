#include "ffplayer.h"
#include "ui_ffplayer.h"


FFPlayer::FFPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FFPlayer)
{
    ui->setupUi(this);

    av_log_set_level(AV_LOG_INFO);   // AV_LOG_ERROR AV_LOG_WARNING AV_LOG_INFO AV_LOG_DEBUG
#if 0
    qDebug() <<avcodec_configuration();
    unsigned version = avcodec_version();
    qDebug() <<"version:" <<version;
#endif
}

FFPlayer::~FFPlayer()
{
    delete ui;
}



