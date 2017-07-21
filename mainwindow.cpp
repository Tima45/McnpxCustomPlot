#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initPlot();
    loader.tallyes = &tallyes;
    loader.moveToThread(&loadingThread);
    loadingThread.start();
    connect(&loader,SIGNAL(errorWhileLoading(QString)),this,SLOT(errorWhileLoadingHendler(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(sendToLoader(QString)),&loader,SLOT(loadTallyes(QString)),Qt::QueuedConnection);
    connect(&loader,SIGNAL(progressUpdate(int)),this,SLOT(updateProgress(int)),Qt::QueuedConnection);
    connect(&loader,SIGNAL(loadingFinished()),this,SLOT(loadingFinished()));



}

MainWindow::~MainWindow()
{
    delete ui;
    for(Tally *tally : tallyes){
        tally->deleteLater();
    }
    loadingThread.wait();
    loadingThread.quit();
}

void MainWindow::on_openFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"Open mcnpx plot file",ui->pathEdit->text());
    ui->pathEdit->setText(filePath);
    loadFile(filePath);
}

void MainWindow::loadFile(QString filePath)
{
    QFileInfo fileInfo(filePath);
    if(fileInfo.isFile()){
        emit sendToLoader(filePath);
    }
}

void MainWindow::errorWhileLoadingHendler(QString text)
{
    QMessageBox::critical(this,"Error",text);
}

void MainWindow::updateProgress(int val)
{
    ui->progressBar->setValue(val);
}

void MainWindow::loadingFinished()
{
    ui->progressBar->setValue(0);
    displayTallyes();
}

void MainWindow::displayTallyes()
{
    if(!tallyes.isEmpty()){
        ui->tallyList->clear();
        for(Tally *tally: tallyes){
            ui->tallyList->addItem(tally->name);
        }
    }
}

void MainWindow::on_tallyList_itemActivated(QListWidgetItem *item)
{
    if(!tallyes.isEmpty()){
        int index = ui->tallyList->row(item);
        currentTally = tallyes.at(index);
    }else{
        currentTally = nullptr;
    }
    drawCurrentTally();
}

void MainWindow::drawCurrentTally()
{
    if(currentTally != nullptr){
        ui->levelBox->setMinimum(currentTally->zRange.first());
        ui->levelBox->setMaximum(currentTally->zRange.last());
        ui->levelBox->setValue(currentTally->zRange.first());
        dysplayAtLevel(currentTally->zRange.first());
    }
}

void MainWindow::dysplayAtLevel(int level)
{
    if(currentTally != nullptr){
        int xCount = currentTally->xRange.count()-1;
        int yCount = currentTally->yRange.count()-1;
        int levelPosition = level+(-currentTally->zRange.first());

        colorMap->data()->setSize(xCount,yCount);
        colorMap->data()->setRange(QCPRange(currentTally->xRange.first(),currentTally->xRange.last()),QCPRange(currentTally->yRange.first(),currentTally->yRange.last()));


        ui->plot->xAxis->setRange(currentTally->xRange.first(),currentTally->xRange.last());
        ui->plot->yAxis->setRange(currentTally->yRange.first(),currentTally->yRange.last());


        for(int yIndex = 0; yIndex < yCount; yIndex++){
            for(int xIndex = 0; xIndex < xCount; xIndex++){
                double value = currentTally->vals.at((levelPosition*xCount*yCount)+(yIndex*xCount)+xIndex);
                colorMap->data()->setCell(xIndex,yIndex,value);
            }
        }

        colorMap->rescaleDataRange();
        ui->plot->replot();
        //-----------------------------------------------------------------------
    }
}
void MainWindow::initPlot()
{
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    //-----------------------------------------------------------------------
    colorScale = new QCPColorScale(ui->plot);
    colorScale->setType(QCPAxis::atRight);
    /*
    g.clearColorStops();
    g.setColorInterpolation(QCPColorGradient::ColorInterpolation::ciRGB);
    g.setColorStopAt(0,QColor(0,0,255));
    g.setColorStopAt(0.5,QColor(255,255,255));
    g.setColorStopAt(1,QColor(255,0,0));*/

    //g.setLevelCount(50);
    colorScale->setGradient(QCPColorGradient::gpThermal);
    colorScale->setDataRange(QCPRange(-1,1));
    ui->plot->plotLayout()->addElement(0,1,colorScale);
    //-----------------------------------------------------------------------
    colorMap = new QCPColorMap(ui->plot->xAxis,ui->plot->yAxis);
    colorMap->setColorScale(colorScale);
    colorMap->setTightBoundary(true);

}

void MainWindow::on_levelBox_valueChanged(int arg1)
{
    dysplayAtLevel(arg1);
}
