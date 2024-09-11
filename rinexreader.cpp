#include "rinexreader.h"

#include "rinex3obs.h"
#include "rinex3nav.h"
#include "timeutils.h"


RinexReader::RinexReader()
{
    rinex_version_obs = -1;
    rinex_version_nav = -1;
    rinex_type_obs = -1;
    rinex_type_nav = -1;
    nav_counter = 0;
}

RinexReader::RinexReader(QString path, RinexType type) : RinexReader()
{
    init(path, type);
}

RinexReader::RinexReader(QString path_obs, QString path_nav) :  RinexReader()
{
    init(path_obs, RinexType::OBSERVATION);
    init(path_nav, RinexType::NAVIGATION);
}

RinexReader::RinexReader(QStringList paths_nav) : RinexReader()
{
    this->paths_nav = paths_nav;
    init(paths_nav.at(0),RinexType::NAVIGATION);
}

RinexReader::RinexReader(QString path_obs, QStringList paths_nav) : RinexReader(paths_nav)
{
    init(path_obs, RinexType::OBSERVATION);
}

RinexReader::~RinexReader()
{
    close();
}

RinexReader::RinexReader(const RinexReader &other) : RinexReader(other.getPath_obs(),other.getPaths_nav()){
    nav_counter = other.nav_counter;
    this->obs = other.obs;
    this->nav = other.nav;
}

RinexReader &RinexReader::operator=(const RinexReader &other)
{
    if (this != &other){
        close();
        this->setPath_obs(other.path_obs);
        this->setPaths_nav(other.paths_nav);
        nav_counter = other.nav_counter;
        this->obs = other.obs;
        this->nav = other.nav;
    }
    return *this;
}




bool RinexReader::readObsHeader()
{
    if(path_obs.isEmpty() || !fin_obs.is_open())
        return false;
    if(checkVersion(RinexType::OBSERVATION)){
        obs.obsHeader(fin_obs);
        //clear flags
        fin_obs.clear();
        //move to beginning
        fin_obs.seekg(0,fin_obs.beg);
        return true;
    }
    return false;
}

QList<Rinex3Obs::ObsEpochInfo> RinexReader::getEpochs()
{
    QList<Rinex3Obs::ObsEpochInfo> epochs;
    if (!path_obs.isEmpty() && fin_obs.is_open()){
        obs.clear(obs._Header);

        obs.obsHeader(fin_obs);

        while (!(fin_obs >> std::ws).eof())
        {
            if (fin_obs.fail())
                break;
            if(obs.obsEpoch(fin_obs)){
                epochs.append(obs._EpochObs);
            }
        }
        obs.clear(obs._EpochObs);
    }
    return epochs;
}




void RinexReader::nextNav()
{
    if (paths_nav.isEmpty() || nav_counter >= paths_nav.size())
        return;
    if (nav_counter != 0)
        init(paths_nav.at(nav_counter),RinexType::NAVIGATION);
    if(checkVersion(RinexType::NAVIGATION) || !fin_nav.is_open()){
        nav.readMixed(fin_nav);
        fin_nav.close();
        nav_counter++;
    }
}

bool RinexReader::readNav(QString path)
{
    if (!paths_nav.contains(path))
        return false;
    init(path,RinexType::NAVIGATION);
    if(checkVersion(RinexType::NAVIGATION)){
        nav.readMixed(fin_nav);
        return true;
    }
    return false;
}

void RinexReader::readNav(int index)
{
    if (index < 0 || index >= paths_nav.size())
        return;
    readNav(paths_nav.at(index));
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
}


QString RinexReader::getPath_obs() const
{
    return path_obs;
}

Rinex3Obs RinexReader::getObs() const
{
    return obs;
}

Rinex3Nav RinexReader::getNav() const
{
    return nav;
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
    clearObs();
    init(newPath_obs, RinexType::OBSERVATION);
}

void RinexReader::setPaths_nav(const QStringList &newPaths_nav)
{
    clearNav();
    paths_nav = newPaths_nav;
    nav_counter = 0;
    init(paths_nav.at(0),RinexType::NAVIGATION);
}

void RinexReader::addPath_nav(QString path)
{
    if (paths_nav.isEmpty())
        init(path,RinexType::NAVIGATION);
    if(!paths_nav.contains(path))
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

//open stream for file(path) and add to field if not contains
void RinexReader::init(QString path, RinexType type)
{
    switch (type) {
    case RinexType::OBSERVATION:
        FIO.fileSafeIn(path.toStdString(), fin_obs);
        if(!fin_obs.is_open())
            return;

        this->path_obs = path;

        FIO.checkRinexVersionType(rinex_version_obs, rinex_type_obs, fin_obs);
        break;
    case RinexType::NAVIGATION:
        if(fin_nav.is_open())
            fin_nav.close();
        FIO.fileSafeIn(path.toStdString(), fin_nav);
        if (!fin_nav.is_open())
            return;

        if(!paths_nav.contains(path))
            paths_nav.append(path);

        FIO.checkRinexVersionType(rinex_version_nav, rinex_type_nav, fin_nav);
        break;
    default:
        break;
    }
}





