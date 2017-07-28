#include "mainwindow.h"
#include "ui_mainwindow.h"

int roundToDivByFour(int value){
    double t = value/4.0;
    t = ceil(t);
    return (int)(t*4);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    ui->xSliceLabel->setScaledContents(true);
    ui->ySliceLabel->setScaledContents(true);
    ui->zSliceLabel->setScaledContents(true);

    doIt1();
}

MainWindow::~MainWindow()
{
    for(Tally *tally : tallyes){
        tally->deleteLater();
    }

    loadingThread->terminate();
    delete loader;
    delete loadingThread;
    delete scatter;
    delete ui;
}

void MainWindow::doIt1()
{
    Tally *newTally = new Tally();
    newTally->name = "test";

    double xmin = -15;
    double xmax = 15;
    double ymin = -15;
    double ymax = 15;
    double zmin = -10;
    double zmax = 20;

    int zCount = 120;
    int yCount = 120;
    int xCount = 120;

    double dx = (xmax-xmin)/(xCount);
    double dy = (ymax-ymin)/(yCount);
    double dz = (zmax-zmin)/(zCount);

    newTally->xRange.append(xmin);
    for(int i = 1; i < xCount+1; i++){
        newTally->xRange.append(newTally->xRange.last()+dx);
    }
    newTally->yRange.append(ymin);
    for(int i = 1; i < yCount+1; i++){
        newTally->yRange.append(newTally->yRange.last()+dy);
    }
    newTally->zRange.append(zmin);
    for(int i = 1; i < zCount+1; i++){
        newTally->zRange.append(newTally->zRange.last()+dz);
    }
    qDebug() << newTally->xRange;
    qDebug() << newTally->xRange.count();

    double xTumour = 0;
    double yTumour = 0;
    double zTumour = 7.8;
    double r1 = 1.5;

    double xh1 = 0;
    double yh1 = 0;
    double zh1 = 3.5;

    double xh2 = 0;
    double yh2 = 0;
    double zh2 = 2.5;

    double xh3 = 0;
    double yh3 = 0;
    double zh3 = 2.5;



    double kx = (xCount-1)/(newTally->xRange.last()-newTally->xRange.first());
    double bx = kx*(-newTally->xRange.first());

    double ky = (yCount-1)/(newTally->yRange.last()-newTally->yRange.first());
    double by = ky*(-newTally->yRange.first());

    double kz = (zCount-1)/(newTally->zRange.last()-newTally->zRange.first());
    double bz = kz*(-newTally->zRange.first());


    for(int z = 0; z < zCount; z++){
        for(int y = 0; y < yCount; y++){
            for(int x = 0; x < xCount; x++){
                newTally->vals.append(0);
                double currentx = ((x)-bx)/kx;
                double currenty = ((y)-by)/ky;
                double currentz = ((z)-bz)/kz;

                double r11 = sqrt(powf(currentx-xTumour,2)+powf(currenty-yTumour,2)+powf(currentz-zTumour,2));
                double r22 = powf((currentx-xh1)/6.0,2)+powf((currenty-yh1)/9.0,2)+powf((currentz-1-zh1)/6.5,2);
                double r33 = powf((currentx-xh2)/6.8,2)+powf((currenty-yh2)/9.8,2)+powf((currentz-zh2)/8.3,2);
                double r44 = powf((currentx-xh3)/7.3,2)+powf((currenty-yh3)/10.3,2)+powf((currentz-zh3)/8.8,2);
                double dd = 0.05;
                if(r11 < r1+dd && r11 > r11-dd){
                    newTally->vals[newTally->vals.count()-1] = (255);
                }

                if(r22 < 1+dd && r22 > 1-dd){
                    newTally->vals[newTally->vals.count()-1] = (128);
                }
                if(r33 < 1+dd && r33 > 1-dd){
                    newTally->vals[newTally->vals.count()-1] = (64);
                }
                if(r44 < 1+dd && r44 > 1-dd){
                    newTally->vals[newTally->vals.count()-1] = (32);
                }
            }
        }
    }
    newTally->maxAbsValue = 255;
    tallyes.append(newTally);
    displayTallyes();
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
        dysplayAt3D();
    }
}


void MainWindow::dysplayAt3D()
{
    if(currentTally != nullptr){
        //OMG!
        int xReal = currentTally->xRange.count()-1;
        qDebug() << xReal;
        int zReal = currentTally->zRange.count()-1;
        int yReal = currentTally->yRange.count()-1;
        int xCount = roundToDivByFour(currentTally->xRange.count()-1);
        int yCount = roundToDivByFour(currentTally->yRange.count()-1);
        int zCount = roundToDivByFour(currentTally->zRange.count()-1);
        int xExtraBlocks = xCount-xReal;
        int yExtraBlocks = yCount-yReal;
        int zExtraBlocks = zCount-zReal;
        double xBlockSize = fabs(currentTally->xRange.last() - currentTally->xRange.first())/xCount;
        double yBlockSize = fabs(currentTally->yRange.last() - currentTally->yRange.first())/yCount;
        double zBlockSize = fabs(currentTally->zRange.last() - currentTally->zRange.first())/zCount;
        double xExtraSpace = xExtraBlocks*xBlockSize;
        double yExtraSpace = yExtraBlocks*yBlockSize;
        double zExtraSpace = zExtraBlocks*zBlockSize;
        scatter->axisX()->setRange(currentTally->xRange.first(),currentTally->xRange.last());
        scatter->axisY()->setRange(currentTally->zRange.first(),currentTally->zRange.last());
        scatter->axisZ()->setRange(currentTally->yRange.first(),currentTally->yRange.last());
        double xSize = (currentTally->xRange.last() - currentTally->xRange.first())+xExtraSpace;
        double ySize = (currentTally->yRange.last() - currentTally->yRange.first())+yExtraSpace;
        double zSize = (currentTally->zRange.last() - currentTally->zRange.first())+zExtraSpace;
        volume->setScaling(QVector3D(xSize,ySize,zSize));
        double xCenter = (scatter->axisX()->max() + scatter->axisX()->min()) / 2.0f;
        double yCenter = (scatter->axisY()->max() + scatter->axisY()->min()) / 2.0f;
        double zCenter = (scatter->axisZ()->max() + scatter->axisZ()->min()) / 2.0f;
        xCenter += xExtraSpace/2.0;
        yCenter -= yExtraSpace/2.0;
        zCenter += zExtraSpace/2.0;
        volume->setPosition(QVector3D(xCenter,yCenter,zCenter));
        volume->setScalingAbsolute(false);
        volume->setTextureWidth(xCount);
        volume->setTextureHeight(yCount);
        volume->setTextureDepth(zCount);
        QVector<uchar> newData;
        for(int zIndex = 0; zIndex < zCount; zIndex++){
            for(int yIndex = 0; yIndex < yCount; yIndex++){
                for(int xIndex = 0; xIndex < xCount; xIndex++){
                    newData.append(0);
                }
            }
        }
        for(int zIndex = 0; zIndex < zReal; zIndex++){
            for(int yIndex = 0; yIndex < yReal; yIndex++){
                for(int xIndex = 0; xIndex < xReal; xIndex++){

                    int dataPosition = (zIndex*xReal*yReal)+(yIndex*xReal)+xIndex;
                    int place = (zIndex*xCount*yCount)+(yIndex*xCount)+xIndex;
                    double value = currentTally->vals.at(dataPosition);
                    newData[place] = (uchar)(255*fabs(value)/currentTally->maxAbsValue);
                }
            }
        }
        volume->setTextureData(new QVector<uchar>(newData));
        volume->setRotation(QQuaternion::fromAxisAndAngle(1,0,0,90));
        ui->xSliceSlider->setMaximum(xReal);
        ui->ySliceSlider->setMaximum(yReal);
        ui->zSliceSlider->setMaximum(zReal);
    }
}

void MainWindow::initScatter()
{
    g = QCPColorGradient(QCPColorGradient::gpThermal);

    scatter = new Q3DScatter;

    scatter->setOrthoProjection(true);
    scatter->activeTheme()->setType(Q3DTheme::ThemeStoneMoss);
    scatter->activeTheme()->setBackgroundEnabled(false);
    scatter->activeTheme()->setLightStrength(0);
    scatter->activeTheme()->setLabelBackgroundEnabled(false);

    scatter->axisX()->setTitle("X");
    scatter->axisY()->setTitle("Y");
    scatter->axisZ()->setTitle("Z");
    scatter->setAspectRatio(1);


    scatter->axisX()->setTitleVisible(true);
    scatter->axisY()->setTitleVisible(true);
    scatter->axisZ()->setTitleVisible(true);
    scatter->setOrthoProjection(true);
    //-------
    QWidget *w = QWidget::createWindowContainer(scatter);
    w->setParent(ui->scatterContainer);
    QGridLayout *l = new QGridLayout(ui->scatterContainer);
    l->addWidget(w);
    ui->scatterContainer->setLayout(l);
    //----------------
    volume = new QCustom3DVolume;

    for(int i = 0; i < 256; i++){
        QColor c = QColor(g.color(i,QCPRange(0,256)));
        colorTableSolid.append(c.rgb());
        c.setAlpha(i);
        colorTableTransparent.append(c.rgba());
    }

    colorTableSolid[0] = QColor(0,0,0,0).rgba();
    volume->setTextureFormat(QImage::Format_Indexed8);
    volume->setColorTable(colorTableSolid);
    scatter->addCustomItem(volume);
}

void MainWindow::on_transparentCkeck_clicked()
{
    if(ui->transparentCkeck->isChecked()){
        volume->setColorTable(colorTableTransparent);
    }else{
        volume->setColorTable(colorTableSolid);
    }
}

void MainWindow::on_showSliceCheck_clicked()
{
    if(ui->showSliceCheck->isChecked()){
        volume->setDrawSlices(true);
    }else{
        volume->setDrawSlices(false);
    }
}

void MainWindow::on_xSliceSlider_valueChanged(int value)
{
    if(currentTally != nullptr){
        if(ui->showSliceCheck->isChecked()){
            volume->setSliceIndexX(value);
        }

        QPixmap p = QPixmap::fromImage(volume->renderSlice(Qt::XAxis,value));

        QTransform t;
        t.rotate(-90);
        p = p.transformed(t);
        ui->xSliceLabel->setPixmap(p);

    }
}

void MainWindow::on_ySliceSlider_valueChanged(int value)
{
    if(currentTally != nullptr){
        if(ui->showSliceCheck->isChecked()){
            volume->setSliceIndexY(value);
        }
        QPixmap p = QPixmap::fromImage(volume->renderSlice(Qt::YAxis,value));
        /*
        QTransform t;
        t.rotate(-90);
        p = p.transformed(t);*/
        ui->ySliceLabel->setPixmap(p);
    }
}

void MainWindow::on_zSliceSlider_valueChanged(int value)
{
    if(currentTally != nullptr){
        if(ui->showSliceCheck->isChecked()){
            volume->setSliceIndexZ(value);
        }
        QPixmap p = QPixmap::fromImage(volume->renderSlice(Qt::ZAxis,value));

        QTransform t;
        t.rotate(180);

        p = p.transformed(t);
        QImage pp = p.toImage();
        p = QPixmap::fromImage(pp.mirrored(true,false));
        ui->zSliceLabel->setPixmap(p);
    }
}
