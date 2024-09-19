#ifndef CSVCREATOR_H
#define CSVCREATOR_H

#include <QString>
#include <QList>
#include <rinex3nav.h>
#include <rinex3obs.h>

class CSVCreator
{
public:
    CSVCreator(QString sep);
    ~CSVCreator();

    virtual void createCSV(QString pathToSave) = 0;

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

    void createCSV(QString pathToSave);
private:
    QList<Rinex3Obs::ObsEpochInfo> epochs;
};
//========================================================================

class CSVnav : public CSVCreator
{
public:
    CSVnav(const Rinex3Nav &nav);
    CSVnav(const Rinex3Nav &nav, QString sep);
    ~CSVnav();

    void createCSV(QString pathToSave);
private:
    Rinex3Nav nav;
};

#endif // CSVCREATOR_H
