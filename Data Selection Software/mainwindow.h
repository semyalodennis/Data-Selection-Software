/*This file is part of the Data Selection software for Hyperspectral images/data.

DEPARTMENT OF BIOSYSTEMS MACHINERY ENGINEERING
NON DESTRUCTIVE BIOSENSING LABORATORY
CHUNGNAM NATIONAL UNIVERSITY

SUPERVISOR: PROFESSOR BYOUNG-KWAN CHO
DEVELOPER: SEMYALO DENNIS*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_actionQuit_triggered();

    void on_actionData_Selection_triggered();

    void on_PB_Load_Cube_clicked();

    void on_PB_Select_Groups_clicked();

    void on_CB_Dark_stateChanged(int arg1);

    void on_CB_White_clicked(bool checked);

    void on_PB_ImgCalib_clicked();

    void on_actionOpen_HSI_DataCube_triggered();

    void on_PB_ViewBand_clicked();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
