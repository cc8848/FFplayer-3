#include "ffplayer.h"
#include "ui_ffplayer.h"


FFPlayer::FFPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FFPlayer)
{
    ui->setupUi(this);



}

FFPlayer::~FFPlayer()
{
    delete ui;
}



