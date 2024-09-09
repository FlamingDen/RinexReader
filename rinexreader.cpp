#include "rinexreader.h"

#include "facadedb.h"
#include "rinex3obs.h"
#include "rinex3nav.h"
#include "timeutils.h"


RinexReader::RinexReader()
{
    rinex_version_obs = -1;
    rinex_version_nav = -1;
    rinex_type_obs = -1;
    rinex_type_nav = -1;
}

RinexReader::RinexReader(QString path, RinexType type)
{
    RinexReader();
    init(path, type);
}

RinexReader::RinexReader(QString path, RinexType type, FacadeDB *db): db(db)
{
    RinexReader(path,type);
}

RinexReader::RinexReader(QString path_obs, QString path_nav)
{
    RinexReader();
    init(path_obs, RinexType::OBSERVATION);
    init(path_nav, RinexType::NAVIGATION);
}

RinexReader::RinexReader(QString path_obs, QString path_nav, FacadeDB *db):
    path_obs(path_obs),db(db) {
    // Setting up the pointers, handling errors
    FIO.fileSafeIn(path_obs.toStdString(), fin_obs);
    FIO.fileSafeIn(path_nav.toStdString(), fin_nav);
    if(fin_nav.is_open())
        paths_nav.append(path_nav);
    FIO.checkRinexVersionType(rinex_version_obs, rinex_type_obs, fin_obs);
    FIO.checkRinexVersionType(rinex_version_nav, rinex_type_nav, fin_nav);
}

RinexReader::RinexReader(QString path_obs, QStringList paths_nav, FacadeDB *db):
    path_obs(path_obs),paths_nav(paths_nav),db(db)
{
    FIO.fileSafeIn(path_obs.toStdString(), fin_obs);
    FIO.fileSafeIn(paths_nav.at(0).toStdString(), fin_nav);
    FIO.checkRinexVersionType(rinex_version_obs, rinex_type_obs, fin_obs);
    FIO.checkRinexVersionType(rinex_version_nav, rinex_type_nav, fin_nav);
}

RinexReader::~RinexReader()
{
    close();
}



bool RinexReader::uploadObsToDB() {
    if(checkVersion(RinexType::OBSERVATION)){
        QDateTime st = QDateTime::currentDateTime();

        readRinex3(fin_obs,fin_nav);

        QDateTime finish = QDateTime::currentDateTime();
        double min;
        int secs, msecs;
        secs = st.secsTo(finish);
        st = st.addSecs(secs);
        msecs = st.time().msecsTo(finish.time());
        std::modf(secs / 60, &min);
        secs -= min * 60;
        std::cout << min <<" min. "<<secs << " sec. " << msecs << " ms."<< std::endl;
        return true;
    }
    return false;
}
bool RinexReader::uploadNavToDB()
{

}



void RinexReader::readObs()
{
    readObsHeader();
    readEpochs();
}
bool RinexReader::readObsHeader()
{
    if(path_obs.isEmpty())
        return false;
    if(checkVersion(RinexType::OBSERVATION)){
        obs.obsHeader(fin_obs);
        return true;
    }
    return false;
}
void RinexReader::readEpochs()
{
    obs.obsEpoch(fin_obs);
}



bool RinexReader::readNav()
{
    if(checkVersion(RinexType::NAVIGATION)){
        for (int i = 0; i < paths_nav.size(); ++i ){
            if (i == 0){
                nav.readMixed(fin_nav);
                continue;
            }
            FIO.fileSafeIn(paths_nav.at(i).toStdString(), fin_nav);
            FIO.checkRinexVersionType(rinex_version_nav, rinex_type_nav, fin_nav);
            nav.readMixed(fin_nav);
        }
        return true;
    }
    return false;
}
bool RinexReader::readNav(QString path)
{
    init(path,RinexType::NAVIGATION);
    if(checkVersion(RinexType::NAVIGATION)){
        nav.readMixed(fin_nav);
        return true;
    }
    return false;
}
bool RinexReader::readNav(QStringList paths)
{
    paths_nav.clear();
    for(const QString &p:paths){
        if(!readNav(p))
            return false;
    }
    return true;
}

void RinexReader::clearObs()
{
    obs.clear(obs._Header);
    obs.clear(obs._EpochObs);
    obs._obsGAL.clear();
    obs._obsGLO.clear();
    obs._obsGPS.clear();
    obs._obsTypesGAL.clear();
    obs._obsTypesGLO.clear();
    obs._obsTypesGPS.clear();
    rinex_type_obs = NULL;
    rinex_version_obs = NULL;
    path_obs = "";
}

void RinexReader::clearNav()
{
    nav.clear();
    rinex_type_nav = NULL;
    rinex_version_nav = NULL;
    paths_nav.clear();
}



void RinexReader::close()
{
    clearObs();
    clearNav();
    this->paths_nav.clear();
    fin_nav.close();
    fin_obs.close();
    db = nullptr;
}
void RinexReader::readRinex3(std::ifstream &fin_obs, std::ifstream &fin_nav)
{
    // Read and store ephemeris file
    nav.readMixed(fin_nav);

    // Read Obs Header from input file
    obs.obsHeader(fin_obs);

    // *** LOOP THROUGH OBSERVATION FILE IN EPOCHWISE MANNER
    while (!(fin_obs >> std::ws).eof()) {
        // *** Deal with end of file error
        if (fin_obs.fail()) { break; }
        // ***
        // Read Epoch
        //OBS._EpochObs.clear();
        obs.obsEpoch(fin_obs);

        // Epoch Information
        std::string obsTimeHMS = HHMMSS(obs._EpochObs.epochRecord[3], obs._EpochObs.epochRecord[4], obs._EpochObs.epochRecord[5]);
        std::cout << "OBS Time : " << obsTimeHMS;

        std::map<std::string, std::map<int,std::vector<double>>> currEpoch = obs._EpochObs.observations;
        std::map<std::string, std::map<int,std::vector<double>>>::iterator itEpoch = currEpoch.begin();
        for(itEpoch = currEpoch.begin(); itEpoch != currEpoch.end(); itEpoch++){
            std::map<int,std::vector<double>> data = itEpoch->second;
            std::map<int,std::vector<double>>::iterator it = data.begin();
            for(it = data.begin(); it != data.end();it++){
                QString code = QString(itEpoch->first.data()) + QString("%1").arg(it->first,2,10,QChar('0'));
                std::vector<double> time = obs._EpochObs.epochRecord;
                QDateTime dt(QDate(time.at(0),time.at(1),time.at(2)),QTime(time.at(3),time.at(4),time.at(5)));
                QList<double> qdate(it->second.begin(),it->second.end());

                if(!db->isExistSL(code))
                    db->addSatellite(code);
                db->addData(dt,code,qdate);
            }
        }
        std::cout << " + \n";
    }
}



QString RinexReader::getPath_obs() const
{
    return path_obs;
}
Rinex3Obs RinexReader::getObs() const
{
    return obs;
}
FacadeDB* RinexReader::getDb() const
{
    return db;
}
FileIO RinexReader::getFIO() const
{
    return FIO;
}
std::ifstream& RinexReader::getFin_obs()
{
    return fin_obs;
}
std::ifstream& RinexReader::getFin_nav()
{
    return fin_nav;
}
QStringList RinexReader::getPaths_nav() const
{
    return paths_nav;
}
double RinexReader::getRinex_version_obs() const
{
    return rinex_version_obs;
}
double RinexReader::getRinex_version_nav() const
{
    return rinex_version_nav;
}
int RinexReader::getRinex_type_obs() const
{
    return rinex_type_obs;
}
int RinexReader::getRinex_type_nav() const
{
    return rinex_type_nav;
}

void RinexReader::setPath_obs(QString newPath_obs)
{
    init(newPath_obs, RinexType::OBSERVATION);
}
void RinexReader::setPaths_nav(const QStringList &newPaths_nav)
{
    paths_nav = newPaths_nav;
    init(paths_nav.at(0),RinexType::NAVIGATION);
}
void RinexReader::setDb(FacadeDB *newDb)
{
    db = newDb;
}
void RinexReader::addPath_nav(QString path)
{
    if (paths_nav.isEmpty()){
        init(path,RinexType::NAVIGATION);
        return;
    }
    paths_nav.append(path);
}



bool RinexReader::checkVersion(RinexType type)
{
    double rinex_version;
    rinex_version = type == RinexType::OBSERVATION ? rinex_version_obs : rinex_version_nav;
    if(rinex_version >= 3 && rinex_version < 4)
        return true;
    if(rinex_version >= 2 && rinex_version < 3)
        std::cout << "Unfortunately, we not support to read file of rinex verion 2.* and below!! " << std::endl;
    return false;
}
Rinex3Nav RinexReader::getNav() const
{
    return nav;
}
void RinexReader::init(QString path, RinexType type)
{
    switch (type) {
    case RinexType::OBSERVATION:
        FIO.fileSafeIn(path.toStdString(), fin_obs);
        this->path_obs = path;
        FIO.checkRinexVersionType(rinex_version_obs, rinex_type_obs, fin_obs);
        break;
    case RinexType::NAVIGATION:
        FIO.fileSafeIn(path.toStdString(), fin_nav);
        FIO.checkRinexVersionType(rinex_version_nav, rinex_type_nav, fin_nav);
        if(fin_nav.is_open())
            paths_nav.append(path);
        break;
    default:
        break;
    }
}






