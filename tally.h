#ifndef TALLY_H
#define TALLY_H

#include <QObject>
#include <QTextStream>
#include <QVector>
#include <QFile>
class Tally : public QObject
{
    Q_OBJECT
public:
    Tally(QObject *parent = 0);
    QString name;
    QVector<double> xRange;
    QVector<double> yRange;
    QVector<double> zRange;
    QVector<double> vals;
    double maxAbsValue;
signals:

public slots:
    bool loadFromFile(QTextStream &inputStream);
    bool loadSingleFromFile(QString fileName);
};

#endif // TALLY_H
