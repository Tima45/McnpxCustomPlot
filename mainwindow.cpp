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

    /*
    doIt1();
    doIt2();
    */
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

void MainWindow::doIt2()
{
    Tally *resultTally = new Tally();
    resultTally->name = "Result dose";

    Tally *HTD14m = new Tally();
    HTD14m->loadSingleFromFile("D:/users/Tima45/lz/Tima Dose/HTD14m");

    Tally *HTD24m = new Tally();
    HTD24m->loadSingleFromFile("D:/users/Tima45/lz/Tima Dose/HTD24m");

    Tally *HTD34m = new Tally();
    HTD34m->loadSingleFromFile("D:/users/Tima45/lz/Tima Dose/HTD34m");

    Tally *HTD44m = new Tally();
    HTD44m->loadSingleFromFile("D:/users/Tima45/lz/Tima Dose/HTD44m");

    qDebug() << HTD14m->vals.count();


    resultTally->xRange = HTD14m->xRange;
    resultTally->yRange = HTD14m->yRange;
    resultTally->zRange = HTD14m->zRange;

    int xReal = resultTally->xRange.count()-1;
    int zReal = resultTally->zRange.count()-1;
    int yReal = resultTally->yRange.count()-1;


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

    double kx = (xReal-1)/(resultTally->xRange.last()-resultTally->xRange.first());
    double bx = kx*(-resultTally->xRange.first());

    double ky = (yReal-1)/(resultTally->yRange.last()-resultTally->yRange.first());
    double by = ky*(-resultTally->yRange.first());

    double kz = (zReal-1)/(resultTally->zRange.last()-resultTally->zRange.first());
    double bz = kz*(-resultTally->zRange.first());



    double maxAbsVal = 0;

    for(int z = 0; z < zReal; z++){
        for(int y = 0; y < yReal; y++){
            for(int x = 0; x < xReal; x++){

                double currentx = ((x)-bx)/kx;
                double currenty = ((y)-by)/ky;
                double currentz = ((z)-bz)/kz;

                double r11 = sqrt(powf(currentx-xTumour,2)+powf(currenty-yTumour,2)+powf(currentz-zTumour,2));
                double r22 = powf((currentx-xh1)/6.0,2)+powf((currenty-yh1)/9.0,2)+powf((currentz-1-zh1)/6.5,2);
                double r33 = powf((currentx-xh2)/6.8,2)+powf((currenty-yh2)/9.8,2)+powf((currentz-zh2)/8.3,2);
                double r44 = powf((currentx-xh3)/7.3,2)+powf((currenty-yh3)/10.3,2)+powf((currentz-zh3)/8.8,2);

                
                int dataPosition = (z*xReal*yReal)+(y*xReal)+x;

                double HTD14mvalue = HTD14m->vals.at(dataPosition);
                double HTD24mvalue = HTD24m->vals.at(dataPosition);
                double HTD34mvalue = HTD34m->vals.at(dataPosition);
                double HTD44mvalue = HTD44m->vals.at(dataPosition);

                
                if(r11 <= r1){
                    HTD14mvalue *= (5250*3.8);
                }
                if(r22 > r1 && r22 <= zh1){
                    HTD14mvalue *= 1950;
                }
                if(r33 > zh1 && r33 <= zh2){
                    HTD14mvalue *= 1;
                }
                if(r44 > zh2 && r44 <= zh3){
                    HTD14mvalue *= 1625;
                }
                if(r44 < zh3){
                    HTD24mvalue *= 3.2;
                    HTD34mvalue *= 3.2;
                    HTD44mvalue *= 1;
                }

                resultTally->vals.append(HTD14mvalue + HTD24mvalue + HTD34mvalue + HTD44mvalue);
                if(fabs(resultTally->vals.last()) > maxAbsVal){
                    maxAbsVal = fabs(resultTally->vals.last());
                }

            }
        }
    }

    resultTally->maxAbsValue = maxAbsVal;
    tallyes.append(resultTally);
    displayTallyes();

    QFile resultFile("D:/users/Tima45/lz/Tima Dose/Result");
    resultFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&resultFile);
    out << "mcnpx      2.5.0  01/10/17 12:00:00   45    1000000      817482696\n";
    out << " c     Created on: Monday, October 1, 2017 at 12:00  \n";
    out << "ntal     1\n";
    out << "    1\n";
    out << "tally    1   -1   -1\n";
    out << "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n";
    out << "f  1771561    0  121  121  121\n";

    int colsCount = 0;
    out.setFieldWidth(13);
    for(int i = 0; i < resultTally->xRange.count(); i++){
        colsCount++;

        if(colsCount == 6){
            out << QString::number(resultTally->xRange.at(i),'E');
            out.setFieldWidth(0);
            out << "\n";
            out.setFieldWidth(13);
            colsCount = 0;
        }else{
            out << QString::number(resultTally->xRange.at(i),'E')+" ";
        }
    }
    out.setFieldWidth(0);
    out << "\n";
    colsCount = 0;
    out.setFieldWidth(13);
    for(int i = 0; i < resultTally->yRange.count(); i++){
        colsCount++;

        if(colsCount == 6){
            out << QString::number(resultTally->yRange.at(i),'E');
            out.setFieldWidth(0);
            out << "\n";
            out.setFieldWidth(13);
            colsCount = 0;
        }else{
            out << QString::number(resultTally->yRange.at(i),'E')+" ";
        }
    }
    out.setFieldWidth(0);
    out << "\n";
    colsCount = 0;
    out.setFieldWidth(13);
    for(int i = 0; i < resultTally->zRange.count(); i++){
        colsCount++;

        if(colsCount == 6){
            out << QString::number(resultTally->zRange.at(i),'E');
            out.setFieldWidth(0);
            out << "\n";
            out.setFieldWidth(13);
            colsCount = 0;
        }else{
            out << QString::number(resultTally->zRange.at(i),'E')+" ";
        }
    }
    out.setFieldWidth(0);
    out << "\nd        1\n";
    out << "u        1\n";
    out << "s        1\n";
    out << "m        1\n";
    out << "c        1\n";
    out << "e        1\n";
    out << "t        1\n";
    out << "vals\n";
    out.setFieldWidth(21);
    colsCount = 0;
    for(int i = 0; i < resultTally->vals.count(); i++){
        QString str = "  "+QString::number(resultTally->vals.at(i),'E')+" 0.0000";
        out << str;
        colsCount++;
        if(colsCount == 4){
            colsCount = 0;
            out.setFieldWidth(0);
            out << "\n";
            out.setFieldWidth(21);
        }
    }



    resultFile.close();
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

        double xTumour = 0;
        double yTumour = 0;
        double zTumour = 7.8;
        double r1 = 1.5;

        double kx = (xCount-1)/(currentTally->xRange.last()-currentTally->xRange.first());
        double bx = kx*(-currentTally->xRange.first());

        double ky = (yCount-1)/(currentTally->yRange.last()-currentTally->yRange.first());
        double by = ky*(-currentTally->yRange.first());

        double kz = (zCount-1)/(currentTally->zRange.last()-currentTally->zRange.first());
        double bz = kz*(-currentTally->zRange.first());



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


                    double currentx = ((xIndex)-bx)/kx;
                    double currenty = ((yIndex)-by)/ky;
                    double currentz = ((zIndex)-bz)/kz;


                    double r11 = sqrt(powf(currentx-xTumour,2)+powf(currenty-yTumour,2)+powf(currentz-zTumour,2));
                    if(r11 < r1){
                        value*=2;
                    }

                    newData[place] = (uchar)(255*fabs(value)/(2.0*currentTally->maxAbsValue));
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
        if( i < 20){
            c.setAlpha(0);
        }else{
            c.setAlpha(128);
        }
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
