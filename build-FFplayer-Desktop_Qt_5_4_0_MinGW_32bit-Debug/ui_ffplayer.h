/********************************************************************************
** Form generated from reading UI file 'ffplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FFPLAYER_H
#define UI_FFPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FFPlayer
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *FFPlayer)
    {
        if (FFPlayer->objectName().isEmpty())
            FFPlayer->setObjectName(QStringLiteral("FFPlayer"));
        FFPlayer->resize(400, 300);
        menuBar = new QMenuBar(FFPlayer);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        FFPlayer->setMenuBar(menuBar);
        mainToolBar = new QToolBar(FFPlayer);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        FFPlayer->addToolBar(mainToolBar);
        centralWidget = new QWidget(FFPlayer);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        FFPlayer->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(FFPlayer);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        FFPlayer->setStatusBar(statusBar);

        retranslateUi(FFPlayer);

        QMetaObject::connectSlotsByName(FFPlayer);
    } // setupUi

    void retranslateUi(QMainWindow *FFPlayer)
    {
        FFPlayer->setWindowTitle(QApplication::translate("FFPlayer", "FFPlayer", 0));
    } // retranslateUi

};

namespace Ui {
    class FFPlayer: public Ui_FFPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FFPLAYER_H
