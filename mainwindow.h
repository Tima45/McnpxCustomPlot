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
#include <QtDataVisualization/Q3DScatter>

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
    QCPColorScale *colorScale;
    QCPColorMap *colorMap;

    Q3DScatter *scatter;
    QVector<QScatter3DSeries *> seriesScale;
    int seriesScaleCount = 100;

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
    void dysplayAtLevel(int level);
    void dysplayAt3D();
    void initPlot();
    void initScatter();

    void on_levelBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
