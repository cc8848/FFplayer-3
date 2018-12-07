#ifndef FFPLAYER_H
#define FFPLAYER_H

#include <QMainWindow>
#include <QDebug>


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
