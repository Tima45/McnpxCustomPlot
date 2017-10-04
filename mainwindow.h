#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QThread>
#include "plot/qcustomplot.h"
#include "loader.h"
#include "tally.h"
#include <QtDataVisualization>

using namespace QtDataVisualization;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QThread *loadingThread;
    Loader *loader;

    QVector<Tally *> tallyes;
    Tally *currentTally = nullptr;

    QCPColorGradient g;

    Q3DScatter *scatter;
    QCustom3DVolume *volume;
    QVector<QRgb> colorTableTransparent;
    QVector<QRgb> colorTableSolid;
    int seriesScaleCount = 100;

    void doIt1();
    void doIt2();
signals:
    void sendToLoader(QString fileName);
private slots:
    void on_openFileButton_clicked();
    void loadFile(QString filePath);
    void errorWhileLoadingHendler(QString text);
    void updateProgress(int val);
    void loadingFinished();
    void displayTallyes();
    void on_tallyList_itemActivated(QListWidgetItem *item);
    void drawCurrentTally();
    void dysplayAt3D();
    void initScatter();


    void on_transparentCkeck_clicked();

    void on_showSliceCheck_clicked();

    void on_xSliceSlider_valueChanged(int value);

    void on_ySliceSlider_valueChanged(int value);

    void on_zSliceSlider_valueChanged(int value);


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
