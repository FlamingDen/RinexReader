#ifndef ENUMTYPES_H
#define ENUMTYPES_H

#include <QString>

enum class RinexType {
    OBSERVATION,
    NAVIGATION
};
//=======================================================================
enum class SatelliteSystem
{
    None          = 0,
    GPS           = 1,
    Galileo       = 2,
    Glonass       = 3,
    BeiDou        = 4,
    Mixed         = 5
};

QString getSatelliteSystem(int s);

QString getSatelliteSystem(SatelliteSystem s);

QString getSatelliteSystemShort(int s);

QString getSatelliteSystemShort(SatelliteSystem s);

SatelliteSystem getSatelliteSystem(QString sys);

#endif // ENUMTYPES_H
