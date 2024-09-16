#ifndef CSVCREATOR_H
#define CSVCREATOR_H

#include <QString>
#include <QList>
#include "rinex3obs.h"

class CSVCreator
{
public:
    CSVCreator(QString sep);
    ~CSVCreator();

    virtual bool createCSV(QString pathToSave) = 0;

    QString getSep() const;
    void setSep(QString newSep);

protected:
    QString sep;
};
//========================================================================
class CSVobs : public CSVCreator
{
public:
    CSVobs(const QList<Rinex3Obs::ObsEpochInfo> &epochs);
    CSVobs(const QList<Rinex3Obs::ObsEpochInfo> &epochs, QString sep);
    ~CSVobs();

    bool createCSV(QString pathToSave);
private:
    QList<Rinex3Obs::ObsEpochInfo> epochs;
};
//========================================================================
#endif // CSVCREATOR_H
