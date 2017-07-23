#ifndef TALLY_H
#define TALLY_H

#include <QObject>
#include <QTextStream>
#include <QVector>
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
};

#endif // TALLY_H
