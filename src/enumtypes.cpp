#include "enumtypes.h"

QString rr::getSatelliteSystemShort(SatelliteSystem s)
{
    switch (s) {
    case SatelliteSystem::GPS:
        return "G";
    case SatelliteSystem::Galileo:
        return "E";
    case SatelliteSystem::Glonass:
        return "R";
    case SatelliteSystem::BeiDou:
        return "C";
    case SatelliteSystem::Mixed:
        return "M";
    default:
        break;
    }
    return {};
}

QString rr::getSatelliteSystemShort(int s){ return getSatelliteSystemShort(static_cast<SatelliteSystem>(s));}

QString rr::getSatelliteSystem(SatelliteSystem s)
{
    switch (s) {
    case SatelliteSystem::None:
        return "None";
    case SatelliteSystem::GPS:
        return "GPS";
    case SatelliteSystem::Galileo:
        return "GALILEO";
    case SatelliteSystem::Glonass:
        return "GLONASS";
    case SatelliteSystem::BeiDou:
        return "BeiDou";
    case SatelliteSystem::Mixed:
        return "Mixed";
    default:
        break;
    }
    return {};
}

QString rr::getSatelliteSystem(int s){ return getSatelliteSystem(static_cast<SatelliteSystem>(s));}

rr::SatelliteSystem rr::getSatelliteSystem(QString sys)
{
    if (sys == "G")
        return SatelliteSystem::GPS;
    if (sys == "E")
        return SatelliteSystem::Galileo;
    if (sys == "R")
        return SatelliteSystem::Glonass;
    if (sys == "C")
        return SatelliteSystem::BeiDou;
    if (sys == "M")
        return SatelliteSystem::Mixed;
    return SatelliteSystem::None;
}
