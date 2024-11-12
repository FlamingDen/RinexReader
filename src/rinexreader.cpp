#include "rinexreader.h"
#include "csvcreator.h"


rr::RinexReader::RinexReader()
{
    rinex_version_obs = -1;
    rinex_version_nav = -1;
    rinex_type_obs = 0;
    rinex_type_nav = 0;
    nav_counter = 0;
}

rr::RinexReader::RinexReader(QString path) : RinexReader()
{
    init(path);
}

rr::RinexReader::RinexReader(QString path_obs, QString path_nav) :  RinexReader()
{
    init(path_obs);
    init(path_nav);
}

rr::RinexReader::RinexReader(QStringList paths_nav) : RinexReader()
{
    this->paths_nav = paths_nav;
    init(paths_nav.at(0));
}

rr::RinexReader::RinexReader(QString path_obs, QStringList paths_nav) : RinexReader(paths_nav)
{
    init(path_obs);
}

rr::RinexReader::~RinexReader()
{
    close();
}

rr::RinexReader::RinexReader(const RinexReader &other) : RinexReader(other.getPathObs(),other.getPathsNav()){
    close();
    this->setPathObs(other.path_obs);
    this->setPathsNav(other.paths_nav);
    nav_counter = other.nav_counter;
    this->obs = other.obs;
    this->nav = other.nav;
}

rr::RinexReader &rr::RinexReader::operator=(const RinexReader &other)
{
    if (this != &other){
        close();
        this->setPathObs(other.path_obs);
        this->setPathsNav(other.paths_nav);
        nav_counter = other.nav_counter;
        this->obs = other.obs;
        this->nav = other.nav;
    }
    return *this;
}




bool rr::RinexReader::readObsHeader()
{
    if(path_obs.isEmpty() || !fin_obs.is_open())
        return false;
    if(checkVersion(RinexType::OBSERVATION)){
        obs.readHead(fin_obs);
        //clear flags
        fin_obs.clear();
        //move to beginning
        fin_obs.seekg(0,fin_obs.beg);
        return true;
    }
    return false;
}

const rr::Rinex3Obs::ObsHeaderInfo& rr::RinexReader::getObsHeaderInfo()
{
    return obs._Header;
}

const QList<rr::Rinex3Obs::ObsEpochInfo>& rr::RinexReader::getEpochs()
{
    //QList<Rinex3Obs::ObsEpochInfo> epochs;
    if (!path_obs.isEmpty() && fin_obs.is_open()){
        if(!epochs.isEmpty())
            return epochs;

        obs.clear(obs._Header);

        obs.readHead(fin_obs);

        while (!(fin_obs >> std::ws).eof())
        {
            if (fin_obs.fail())
                break;
            obs.readBody(fin_obs);
            epochs.append(obs._EpochObs);
        }
        obs.clear(obs._EpochObs);

        fin_obs.clear();
        fin_obs.seekg(0,fin_obs.beg);
    }
    return epochs;
}

void rr::RinexReader::saveAsCSV(QString pathToSave, RinexType type, QString sep)
{
    switch (type) {
    case RinexType::OBSERVATION: {
        if(!path_obs.isEmpty() && checkVersion(type)){
            CSVobs csv_obs(getEpochs(), sep);
            csv_obs.createCSV(pathToSave);
        }
        break;
    }
    case RinexType::NAVIGATION: {
        if(!paths_nav.isEmpty() && checkVersion(type)){
            CSVnav csv_nav(nav, sep);
            csv_nav.createCSV(pathToSave);
        }
        break;
    }
    default:
        break;
    }
}




void rr::RinexReader::nextNav()
{
    if (paths_nav.isEmpty() || nav_counter >= paths_nav.size())
        return;
    if (nav_counter != 0)
        init(paths_nav.at(nav_counter));
    if(checkVersion(RinexType::NAVIGATION) || !fin_nav.is_open()){
        readNav(nav_counter);
        fin_nav.close();
        nav_counter++;
    }
}

bool rr::RinexReader::readNav(QString path)
{
    if (!paths_nav.contains(path))
        return false;
    init(path);
    if(checkVersion(RinexType::NAVIGATION)){
        switch (rinex_type_nav) {
        case static_cast<int>(SatelliteSystem::GPS):{
            nav.readGPS(fin_nav);
            break;
        }
        case static_cast<int>(SatelliteSystem::Galileo):{
            nav.readGAL(fin_nav);
            break;
        }
        case static_cast<int>(SatelliteSystem::Glonass):{
            nav.readGLO(fin_nav);
            break;
        }
        case static_cast<int>(SatelliteSystem::BeiDou):{
            nav.readBEI(fin_nav);
            break;
        }
        case static_cast<int>(SatelliteSystem::Mixed):{
            nav.readMixed(fin_nav);
            break;
        }
        default:
            return false;
        }
        return true;
    }
    return false;
}

void rr::RinexReader::readNav(int index)
{
    if (index < 0 || index >= paths_nav.size())
        return;
    readNav(paths_nav.at(index));
}



void rr::RinexReader::clearObs()
{
    epochs.clear();

    obs.clear(obs._Header);
    obs.clear(obs._EpochObs);

    obs._obsGAL.clear();
    obs._obsGLO.clear();
    obs._obsGPS.clear();
    obs._obsBEI.clear();
    obs._obsTypesGAL.clear();
    obs._obsTypesGLO.clear();
    obs._obsTypesGPS.clear();
    obs._obsTypesBEI.clear();

    rinex_type_obs = 0;
    rinex_version_obs = -1;
    path_obs = "";
    fin_obs.close();
}

void rr::RinexReader::clearRRNav()
{
    clearNavData();

    rinex_type_nav = 0;
    rinex_version_nav = -1;
    paths_nav.clear();
    fin_nav.close();
}

void rr::RinexReader::clearNavData()
{
    nav.clear();
}



void rr::RinexReader::close()
{
    clearObs();
    clearRRNav();
}


const QString& rr::RinexReader::getPathObs() const
{
    return path_obs;
}

rr::ViewNav rr::RinexReader::getNav() const
{
    ViewNav view_nav = nav;
    return view_nav;
}

const QStringList& rr::RinexReader::getPathsNav() const
{
    return paths_nav;
}

const double& rr::RinexReader::getRinexVersionObs() const
{
    return rinex_version_obs;
}

const double& rr::RinexReader::getRinexVersionNav() const
{
    return rinex_version_nav;
}

const int& rr::RinexReader::getRinexTypeObs() const
{
    return rinex_type_obs;
}

const int& rr::RinexReader::getRinexTypeNav() const
{
    return rinex_type_nav;
}

void rr::RinexReader::setPathObs(QString newPath_obs)
{
    clearObs();
    init(newPath_obs);
}

void rr::RinexReader::setPathsNav(const QStringList &newPaths_nav)
{
    clearRRNav();
    paths_nav = newPaths_nav;
    nav_counter = 0;
    init(paths_nav.at(0));
}

void rr::RinexReader::addPath_nav(QString path)
{
    if (paths_nav.isEmpty()){
        init(path);
        return;
    }
    if(!paths_nav.contains(path))
        paths_nav.append(path);
}

const QString& rr::RinexReader::getCurrPathNav() const
{
    return curr_path_nav;
}



bool rr::RinexReader::checkVersion(RinexType type)
{
    double rinex_version;
    rinex_version = type == RinexType::OBSERVATION ? rinex_version_obs : rinex_version_nav;
    if(rinex_version >= 3.04 && rinex_version < 4)
        return true;
    if(rinex_version >= 2 && rinex_version < 3)
        std::cout << "Unfortunately, we not support to read file of rinex verion 2.* and below!! " << std::endl;
    return false;
}

//open stream for file(path) and add to field if not contains
void rr::RinexReader::init(QString path)
{
    std::ifstream in;
    double version;
    std::string type_file;
    int rinex_type;

    FIO.fileSafeIn(path.toStdString(), in);
    FIO.checkRinexVersionType(version, type_file , rinex_type, in);
    in.close();

    switch (type_file.compare("O") == 0 ? RinexType::OBSERVATION : RinexType::NAVIGATION) {
    case RinexType::OBSERVATION:{
        FIO.fileSafeIn(path.toStdString(), fin_obs);
        if(!fin_obs.is_open())
            return;

        this->path_obs = path;

        rinex_version_obs = version;
        rinex_type_obs = rinex_type;

        readObsHeader();
        break;
    }
    case RinexType::NAVIGATION:{
        if(fin_nav.is_open())
            fin_nav.close();
        FIO.fileSafeIn(path.toStdString(), fin_nav);
        if (!fin_nav.is_open())
            return;

        if(!paths_nav.contains(path))
            paths_nav.append(path);

        rinex_version_nav = version;
        rinex_type_nav = rinex_type;

        curr_path_nav = path;
        break;
    }
    default:
        break;
    }
}




