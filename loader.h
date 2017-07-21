#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QFile>
#include <QVector>
#include "tally.h"
#include <QProgressBar>

class Loader : public QObject
{
    Q_OBJECT
public:
    Loader(QObject *parent = 0);
    QProgressBar *bar;
    QVector<Tally *> *tallyes;
signals:
    void errorWhileLoading(QString text);
    void progressUpdate(int prog);
    void loadingFinished();
public slots:
    bool loadTallyes(QString fileName);

};

#endif // LOADER_H
