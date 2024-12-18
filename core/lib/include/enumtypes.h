#pragma once

#include <QString>

namespace rr
{
    enum class RinexType
    {
        OBSERVATION = 0,
        NAVIGATION  = 1
    };

    //=======================================================================
    enum class SatelliteSystem
    {
        None    = 0,
        GPS     = 1,
        Galileo = 2,
        Glonass = 3,
        BeiDou  = 4,
        Mixed   = 5
    };

    QString getSatelliteSystem(int s);

    QString getSatelliteSystem(SatelliteSystem s);

    QString getSatelliteSystemShort(int s);

    QString getSatelliteSystemShort(SatelliteSystem s);

    SatelliteSystem getSatelliteSystem(QString sys);
}


