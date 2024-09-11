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

    bool readObsHeader();                           //read a header and Set position stream on beginning
    QList<Rinex3Obs::ObsEpochInfo> getEpochs();     //give you list of epochs

    void nextNav();                                 // read in order, one by one
    bool readNav(QString path);                     //
    void readNav(int index);

    void clearObs();
    void clearNav();
    void close();

    QString getPath_obs() const;
    QStringList getPaths_nav() const;
    Rinex3Obs getObs() const;
    Rinex3Nav getNav() const;
    FileIO getFIO() const;
    std::ifstream& getFin_obs();
    std::ifstream& getFin_nav();
    double getRinex_version_obs() const;
    double getRinex_version_nav() const;
    int getRinex_type_obs() const;
    int getRinex_type_nav() const;

    void setPath_obs(QString newPath_obs);
    void setPaths_nav(const QStringList &newPaths_nav);
    void addPath_nav(QString path);

private:
    QString path_obs;
    QStringList paths_nav;
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

    void init(QString path, RinexType type);
    bool checkVersion(RinexType type);
};

#endif // RINEXREADER_H
