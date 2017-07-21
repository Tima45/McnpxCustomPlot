#include "tally.h"

Tally::Tally(QObject *parent) : QObject(parent)
{

}

bool Tally::loadFromFile(QTextStream &inputStream)
{
    QString rawLine = inputStream.readLine();
    QStringList list = rawLine.split(" ",QString::SkipEmptyParts);
    if(!list.isEmpty()){
        if(list.first() == "tally"){
            name = "tally "+list.at(1);
            QString rawLine = inputStream.readLine();
            rawLine = inputStream.readLine();
            QStringList list = rawLine.split(" ",QString::SkipEmptyParts);
            if(!list.isEmpty()){
                if(list.at(0) == "f"){
                    int valsCount = list.at(1).toInt();
                    int xCount = list.at(3).toInt();
                    int yCount = list.at(4).toInt();
                    int zCount = list.at(5).toInt();
                    QString block;
                    QString currentLine;
                    while(!inputStream.atEnd()){
                        currentLine = inputStream.readLine();
                        QStringList list = currentLine.split(" ",QString::SkipEmptyParts);
                        if(list.at(0) == "d") break;
                        block += currentLine;
                    }
                    QStringList list = block.split(" ",QString::SkipEmptyParts);
                    if(list.count() == xCount+yCount+zCount+3){
                        xRange.clear();
                        yRange.clear();
                        zRange.clear();
                        vals.clear();
                        int index = 0;
                        for(; index < xCount+1; index++){
                            xRange.append(list.at(index).toDouble());
                        }
                        for(;index < xCount+yCount+2;index++){
                            yRange.append(list.at(index).toDouble());
                        }
                        for(;index < xCount+yCount+zCount+3;index++){
                            zRange.append(list.at(index).toDouble());
                        }
                        QString line = "";
                        while(line != "vals"){
                            line = inputStream.readLine();
                            if(inputStream.atEnd()) return false;
                        }
                        block.clear();
                        currentLine.clear();
                        while(!inputStream.atEnd()){
                            currentLine = inputStream.readLine();
                            QStringList list = currentLine.split(" ",QString::SkipEmptyParts);
                            if(list.at(0) == "tally"){
                                inputStream.seek(inputStream.pos()-(currentLine.count()+2));
                                break;
                            }
                            block += currentLine;
                        }
                        QStringList list = block.split(" ",QString::SkipEmptyParts);
                        for(int i = 0; i < list.count(); i+=2){
                            vals.append(list.at(i).toDouble());
                        }
                        if(valsCount != vals.count()){
                            return false;
                        }
                    }else{
                        return false;
                    }
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}
