#ifndef RINEXREADER_H
#define RINEXREADER_H

#include <QString>
#include <QObject>
#include "rinex3obs.h"
#include "rinex3nav.h"
#include "fileio.h"

enum class RinexType {
    OBSERVATION,
    NAVIGATION
};

enum class SatelliteSystem
{
    None          = 0,
    GPS           = 1,
    Galileo       = 2,
    Glonass       = 3,
    BeiDou        = 4,
    Mixed         = 5
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

    const Rinex3Obs::ObsHeaderInfo& getObsHeaderInfo(); //give header obs info
    QList<Rinex3Obs::ObsEpochInfo> getEpochs();         //give you list of epochs
    void saveObsAsCSV(QString pathToSave);              //save obs file in .csv format
    void saveObsAsCSV(QString pathToSave, QString sep); //

    void nextNav();                                     // read in order, one by one
    bool readNav(QString path);                         // read anv file by path
    void readNav(int index);                            // read anv file by index
    Rinex3Nav getNav() const;                           // give all data from nav file

    void clearObs();
    void clearNav();

    //Getter
    QString getPath_obs() const;
    QStringList getPaths_nav() const;
    FileIO getFIO() const;
    double getRinex_version_obs() const;
    double getRinex_version_nav() const;
    int getRinex_type_obs() const;
    int getRinex_type_nav() const;
    QString getCurr_path_nav() const;

    //Setter
    void setPath_obs(QString newPath_obs);
    void setPaths_nav(const QStringList &newPaths_nav);

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

    bool readObsHeader();                                   //read a header and Set position stream on beginning

    void close();
    void init(QString path, RinexType type);                //open stream for file(path) and add to field if not contains
    bool checkVersion(RinexType type);
};

#endif // RINEXREADER_H
