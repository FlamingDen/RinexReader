#include "enumtypes.h"

QString getSatelliteSystemShort(SatelliteSystem s)
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

QString getSatelliteSystemShort(int s){ return getSatelliteSystemShort(static_cast<SatelliteSystem>(s));}

QString getSatelliteSystem(SatelliteSystem s)
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

QString getSatelliteSystem(int s){ return getSatelliteSystem(static_cast<SatelliteSystem>(s));}
