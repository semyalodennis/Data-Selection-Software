/*This file is part of the Data Selection software for Hyperspectral images/data.

DEPARTMENT OF BIOSYSTEMS MACHINERY ENGINEERING
NON DESTRUCTIVE BIOSENSING LABORATORY
CHUNGNAM NATIONAL UNIVERSITY

SUPERVISOR: PROFESSOR BYOUNG-KWAN CHO
DEVELOPER: SEMYALO DENNIS*/

#include "mainwindow.h"

#include <QApplication>

//Qt
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/rec/Icons/CNU_logo.png"));
    splash->show();

    MainWindow w;

     QTimer::singleShot(2500, splash, SLOT(close()));
     QTimer::singleShot(2500, &w, SLOT(show()));

    //w.show();

    return a.exec();
}
