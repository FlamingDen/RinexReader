#ifndef RINEXREADER_H
#define RINEXREADER_H

#include <QString>
#include <QObject>
#include "rinex3obs.h"
#include "rinex3nav.h"
#include "fileio.h"
#include "timeutils.h"

enum class RinexType {
    OBSERVATION,
    NAVIGATION
};

class RinexReader
{
public:
    RinexReader();
    RinexReader(QString path, RinexType type);
    RinexReader(QString path_obs,QString path_nav);
    RinexReader(QStringList paths_nav);
    RinexReader(QString path_obs, QStringList paths_nav);
    ~RinexReader();

    RinexReader(const RinexReader &other);
    RinexReader &operator=(const RinexReader& other);

    //bool readObsHeader();                           //read a header and Set position stream on beginning
    const Rinex3Obs::ObsHeaderInfo& getObsHeaderInfo();
    QList<Rinex3Obs::ObsEpochInfo> getEpochs();     //give you list of epochs
    void saveObsAsCSV(QString pathToSave, QString sep);

    void nextNav();                                 // read in order, one by one
    bool readNav(QString path);                     //
    void readNav(int index);
    Rinex3Nav getNav() const;

    void clearObs();
    void clearNav();
    void close();


    QString getPath_obs() const;
    QStringList getPaths_nav() const;
    FileIO getFIO() const;
    double getRinex_version_obs() const;
    double getRinex_version_nav() const;
    QString getRinex_type_obs() const;
    QString getRinex_type_nav() const;

    void setPath_obs(QString newPath_obs);
    void setPaths_nav(const QStringList &newPaths_nav);
    void addPath_nav(QString path);

    QString getCurr_path_nav() const;

private:
    QString path_obs;
    QStringList paths_nav;
    QString curr_path_nav;
    double rinex_version_obs;
    double rinex_version_nav;
    QString rinex_type_obs;
    QString rinex_type_nav;
    Rinex3Obs obs;
    Rinex3Nav nav;
    FileIO FIO;
    std::ifstream fin_obs;
    std::ifstream fin_nav;
    int nav_counter;

    bool readObsHeader();

    void init(QString path, RinexType type);
    bool checkVersion(RinexType type);
};

#endif // RINEXREADER_H
