#ifndef CSVCREATOR_H
#define CSVCREATOR_H

#include <QString>
#include <QList>
#include "rinex3obs.h"

class CSVCreator
{
public:
    CSVCreator(const QList<Rinex3Obs::ObsEpochInfo>& epochs, QString sep);

    bool createCSV(QString pathToSave);
private:
    QString sep;
    QList<Rinex3Obs::ObsEpochInfo> epochs;
};

#endif // CSVCREATOR_H
