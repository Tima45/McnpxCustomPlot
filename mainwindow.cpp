#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initPlot();
    initScatter();
    loader = new Loader;
    loader->tallyes = &tallyes;
    loadingThread = new QThread;
    loader->moveToThread(loadingThread);

    loadingThread->start();
    connect(loader,SIGNAL(errorWhileLoading(QString)),this,SLOT(errorWhileLoadingHendler(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(sendToLoader(QString)),loader,SLOT(loadTallyes(QString)),Qt::QueuedConnection);
    connect(loader,SIGNAL(progressUpdate(int)),this,SLOT(updateProgress(int)),Qt::QueuedConnection);
    connect(loader,SIGNAL(loadingFinished()),this,SLOT(loadingFinished()));
}

MainWindow::~MainWindow()
{
    for(Tally *tally : tallyes){
        tally->deleteLater();
    }
    for(QScatter3DSeries *series : seriesScale){
        delete series;
    }

    loadingThread->terminate();
    delete loader;
    delete loadingThread;
    delete scatter;


    delete ui;
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
        currentTally = nullptr;
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
        dysplayAt3D();
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

void MainWindow::dysplayAt3D()
{
    if(currentTally != nullptr){
        for(int i = 0; i < seriesScale.count(); i++){
            seriesScale.at(i)->dataProxy()->removeItems(0,seriesScale.at(i)->dataProxy()->itemCount());
        }
        scatter->axisX()->setRange(currentTally->xRange.first(),currentTally->xRange.last());
        scatter->axisY()->setRange(currentTally->yRange.first(),currentTally->yRange.last());
        scatter->axisZ()->setRange(currentTally->zRange.first(),currentTally->zRange.last());

        int xCount = currentTally->xRange.count()-1;
        int yCount = currentTally->yRange.count()-1;
        int zCount = currentTally->zRange.count()-1;

        for(int yIndex = 0; yIndex < yCount; yIndex++){
            for(int xIndex = 0; xIndex < xCount; xIndex++){
                for(int zIndex = 0; zIndex < zCount; zIndex++){
                    double value = currentTally->vals.at((zIndex*xCount*yCount)+(yIndex*xCount)+xIndex);
                    if(value != 0){
                        QScatterDataItem *item = new QScatterDataItem;
                        float x = currentTally->xRange.at(xIndex+1);
                        float y = currentTally->yRange.at(yIndex+1);
                        float z = currentTally->zRange.at(zIndex+1);
                        item->setPosition(QVector3D(x,y,z));

                        int colorPosition = qRound((fabs(value)/currentTally->maxAbsValue)*seriesScaleCount);
                        if(colorPosition >= 0 && colorPosition < seriesScaleCount){
                            seriesScale.at(colorPosition)->dataProxy()->addItem(*item);
                        }else{
                            qDebug() << colorPosition << "wrong number";
                            delete item;
                        }
                    }
                }
            }
        }

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
    g = QCPColorGradient(QCPColorGradient::gpThermal);
    colorScale->setGradient(g);
    colorScale->setDataRange(QCPRange(-1,1));
    ui->plot->plotLayout()->addElement(0,1,colorScale);
    //-----------------------------------------------------------------------
    colorMap = new QCPColorMap(ui->plot->xAxis,ui->plot->yAxis);
    colorMap->setColorScale(colorScale);
    colorMap->setTightBoundary(true);

}

void MainWindow::initScatter()
{
    scatter = new Q3DScatter;

    scatter->activeTheme()->setType(Q3DTheme::ThemeStoneMoss);
    scatter->activeTheme()->setBackgroundEnabled(false);
    scatter->activeTheme()->setLightStrength(0);
    scatter->activeTheme()->setLabelBackgroundEnabled(false);

    QWidget *w = QWidget::createWindowContainer(scatter);
    w->setParent(ui->scatterContainer);
    QGridLayout *l = new QGridLayout(ui->scatterContainer);
    l->addWidget(w);
    ui->scatterContainer->setLayout(l);
    //-------
    for(int i = 0; i < seriesScaleCount; i++){
        QScatter3DSeries * newSeries = new QScatter3DSeries();
        newSeries->setMesh(QAbstract3DSeries::MeshPoint);
        newSeries->setBaseColor(QColor(g.color(i,QCPRange(0,seriesScaleCount))));
        scatter->addSeries(newSeries);
        seriesScale.append(newSeries);
    }

}

void MainWindow::on_levelBox_valueChanged(int arg1)
{
    dysplayAtLevel(arg1);
}
