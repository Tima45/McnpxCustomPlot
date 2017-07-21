#include "loader.h"

Loader::Loader(QObject *parent) : QObject(parent)
{

}

bool Loader::loadTallyes(QString fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly | QIODevice::Text)){
        for(Tally * tally : *tallyes){
            tally->deleteLater();
        }
        tallyes->clear();
        //------------------------------------------------------------------
        QTextStream inputTextStream(&f);
        QString rawLine;
        rawLine = inputTextStream.readLine();
        rawLine = inputTextStream.readLine();
        rawLine = inputTextStream.readLine();
        QStringList list = rawLine.split(" ",QString::SkipEmptyParts);
        if(list.count() == 2){
            rawLine = inputTextStream.readLine();

            int newTallyesCount = list.at(1).toInt();
            int currentProgress = 0;
            int dx = 100/newTallyesCount;
            emit progressUpdate(10);
            for(int i = 0; i < newTallyesCount; i++){
                Tally *newTally = new Tally;
                if(!newTally->loadFromFile(inputTextStream)){
                    emit errorWhileLoading("Unknown file format.");
                    break;
                }
                tallyes->append(newTally);
                currentProgress = currentProgress+dx;
                emit progressUpdate(currentProgress);
            }
            f.close();
            emit loadingFinished();
        }else{
            emit errorWhileLoading("Unknown file format.");
        }
    }else{
        emit errorWhileLoading("Could not open file.");
    }
}
