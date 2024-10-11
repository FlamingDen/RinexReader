#ifndef RINEXREADER_H
#define RINEXREADER_H

#include <QList>

#include "rinex3obs.h"
#include "rinex3nav.h"
#include "enumtypes.h"
#include "fileio.h"


class RinexReader
{
public:
    RinexReader();
    RinexReader(QString path);
    RinexReader(QString path_obs,QString path_nav);
    RinexReader(QStringList paths_nav);
    RinexReader(QString path_obs, QStringList paths_nav);
    ~RinexReader();

    RinexReader(const RinexReader &other);
    RinexReader &operator=(const RinexReader& other);

    const Rinex3Obs::ObsHeaderInfo& getObsHeaderInfo(); //give header obs info
    const QList<Rinex3Obs::ObsEpochInfo>& getEpochs();         //give you list of epochs

    void saveAsCSV(QString pathToSave, RinexType type, QString sep = ","); //save files in .csv format

    void nextNav();                                     // read in order, one by one
    bool readNav(QString path);                         // read anv file by path
    void readNav(int index);                            // read anv file by index
    ViewNav getNav() const;                             // give all data from nav file

    void clearObs();
    void clearRRNav();
    void clearNavData();

    //Getter
    const QString& getPathObs() const;
    const QString& getCurrPathNav() const;
    const QStringList& getPathsNav() const;
    const double& getRinexVersionObs() const;
    const double& getRinexVersionNav() const;
    const int& getRinexTypeObs() const;
    const int& getRinexTypeNav() const;

    //Setter
    void setPathObs(QString newPath_obs);
    void setPathsNav(const QStringList &newPaths_nav);

    void addPath_nav(QString path);

private:
    QString path_obs;
    QStringList paths_nav;
    QString curr_path_nav;
    double rinex_version_obs;
    double rinex_version_nav;
    int rinex_type_obs;
    int rinex_type_nav;
    Rinex3Obs obs;
    Rinex3Nav nav;
    FileIO FIO;
    std::ifstream fin_obs;
    std::ifstream fin_nav;
    int nav_counter;
    QString curr_rin_type;

    QVector<QString> satellites;
    QList<Rinex3Obs::ObsEpochInfo> epochs;

    bool readObsHeader();                                   //read a header and Set position stream on beginning

    void close();
    void init(QString path);                //open stream for file(path) and add to field if not contains
    bool checkVersion(RinexType type);
};

#endif // RINEXREADER_H
