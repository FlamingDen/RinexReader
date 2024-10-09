#ifndef CSVCREATOR_H
#define CSVCREATOR_H

#include <QString>
#include <QList>
#include <rinex3obs.h>
#include <rinex3nav.h>


class CSVCreator
{
public:
    virtual void createCSV(QString pathToSave) = 0;

    QString getSep() const;
    void setSep(QString newSep);
protected:
    CSVCreator(QString sep);
    virtual ~CSVCreator() = 0;

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
