#pragma once

#include "timeutils.h"
#include "stringutils.h"
#include <optional>

#ifndef RINEX3NAV_H_
#define RINEX3NAV_H_

//TO DO BEIDOU
class Rinex3Nav
{
public:
    Rinex3Nav();
    ~Rinex3Nav();

    // Data Structures
    //GPS
    struct DataGPS {
        bool isAvailable;
        const int SatelliteSystem = 1;

        //==================SV / EPOCH / SV CLK=============
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double clockDrift;
        double clockDriftRate;

        //==================BROADCAST ORBIT – 1=============
        double IODE;
        double Crs;
        double Delta_n;
        double M0;

        //==================BROADCAST ORBIT – 2=============
        double Cuc;
        double Eccentricity;
        double Cus;
        double Sqrt_a;

        //==================BROADCAST ORBIT – 3=============
        double TOE;
        double Cic;
        double OMEGA0;
        double Cis;

        //==================BROADCAST ORBIT – 4=============
        double i0;
        double Crc;
        double omega;
        double OMEGA_DOT;

        //==================BROADCAST ORBIT – 5=============
        double IDOT;
        double L2_codes_channel;
        double GPS_week;
        double L2_P_data_flag;

        //==================BROADCAST ORBIT – 6=============
        double svAccuracy;
        double svHealth;
        double TGD;
        double IODC;

        //==================BROADCAST ORBIT – 7=============
        double transmission_time;
        std::optional<double> fit_interval;
        std::optional<double> spare1;
        std::optional<double> spare2;

        std::vector<std::optional<double>> toVec();
    };
    //Glonass
    struct DataGLO {
        bool isAvailable;
        const int SatelliteSystem = 3;

        //==================SV / EPOCH / SV CLK=============
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double relFreqBias;
        double messageFrameTime;

        //==================BROADCAST ORBIT – 1=============
        double satPosX;
        double satVelX;
        double satAccX;
        double satHealth;

        //==================BROADCAST ORBIT – 2=============
        double satPosY;
        double satVelY;
        double satAccY;
        double freqNum;

        //==================BROADCAST ORBIT – 3=============
        double satPosZ;
        double satVelZ;
        double satAccZ;
        double infoAge;

        std::vector<std::optional<double>> toVec();
    };
    //Galileo
    struct DataGAL {
        bool isAvailable;
        const int SatelliteSystem = 2;

        //==================SV / EPOCH / SV CLK=============
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double clockDrift;
        double clockDriftRate;

        //==================BROADCAST ORBIT – 1=============
        double IOD;
        double Crs;
        double Delta_n;
        double M0;

        //==================BROADCAST ORBIT – 2=============
        double Cuc;
        double Eccentricity;
        double Cus;
        double Sqrt_a;

        //==================BROADCAST ORBIT – 3=============
        double TOE;
        double Cic;
        double OMEGA0;
        double Cis;

        //==================BROADCAST ORBIT – 4=============
        double i0;
        double Crc;
        double omega;
        double OMEGA_DOT;

        //==================BROADCAST ORBIT – 5=============
        double IDOT;
        double Data_sources;
        double GAL_week;
        std::optional<double> spare1;

        //==================BROADCAST ORBIT – 6=============
        double SISA;
        double svHealth;
        double BGD_E5a;
        double BGD_E5b;

        //==================BROADCAST ORBIT – 7=============
        double transmission_time;
        std::optional<double> spare2;
        std::optional<double> spare3;
        std::optional<double> spare4;
        //also have 3 spare places

        std::vector<std::optional<double>> toVec();
    };
    //BeiDou
    struct DataBEI {
        bool isAvailable;
        const int SatelliteSystem = 4;

        //==================SV / EPOCH / SV CLK=============
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double clockDrift;
        double clockDriftRate;

        //==================BROADCAST ORBIT – 1=============
        double AODE;
        double Crs;
        double Delta_n;
        double M0;

        //==================BROADCAST ORBIT – 2=============
        double Cuc;
        double Eccentricity;
        double Cus;
        double Sqrt_a;

        //==================BROADCAST ORBIT – 3=============
        double TOE;
        double Cic;
        double OMEGA0;
        double Cis;

        //==================BROADCAST ORBIT – 4=============
        double i0;
        double Crc;
        double omega;
        double OMEGA_DOT;

        //==================BROADCAST ORBIT – 5=============
        double IDOT;
        std::optional<double> spare1;
        double BDT_week;
        std::optional<double> spare2;

        //==================BROADCAST ORBIT – 6=============
        double SVaccuracy;
        double SatH1;
        double TGD1;
        double TGD2;

        //==================BROADCAST ORBIT – 7=============
        double transmission_time;
        double AODC;
        std::optional<double> spare3;
        std::optional<double> spare4;

        std::vector<std::optional<double>> toVec();
    };

    // Headers
    struct HeaderGPS {
        // Ionospheric alpha and beta constants
        std::vector<double> ialpha;
        std::vector<double> ibeta;
        // Time System correction
        std::vector<double> GPUT;
        double leapSec;
    };

    struct HeaderGLO {
        // Time System correction
        std::vector<double> TimeCorr;
        double leapSec;
    };

    struct HeaderGAL {
        // Time System correction
        double leapSec;
    };

    struct HeaderBEI {
        // Time System correction
        std::vector<double> TimeCorr;
        double leapSec;
    };

    // Attributes
    // * Navigation data structure mapped to PRN for ease of use
    std::map<int, std::vector<Rinex3Nav::DataGPS>> _navGPS;
    std::map<int, std::vector<Rinex3Nav::DataGLO>> _navGLO;
    std::map<int, std::vector<Rinex3Nav::DataGAL>> _navGAL;
    std::map<int, std::vector<Rinex3Nav::DataBEI>> _navBEI;

    // * Header information
    HeaderGPS _headerGPS;
    HeaderGLO _headerGLO;
    HeaderGAL _headerGAL;
    HeaderBEI _headerBEI;


    // Functions
    void readGPS(std::ifstream& infile); // for separate GPS only navigation files
    void readGLO(std::ifstream& infile); // for separate GLO only navigation files
    void readGAL(std::ifstream& infile); // for separate GAL only navigation files
    void readBEI(std::ifstream& infile); // for separate BEI only navigation files
    void readMixed(std::ifstream& inputfileMixed); // for mixed navigation files
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGPS> NAV);
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGAL> NAV);
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGLO> NAV);
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataBEI> NAV);

    void clear();
};

struct ViewNav
{
    ViewNav();
    ViewNav(const Rinex3Nav &other);
    ViewNav &operator=(const Rinex3Nav &other);

    // Attributes
    // * Navigation data structure mapped to PRN for ease of use
    std::map<int, std::vector<Rinex3Nav::DataGPS>> _navGPS;
    std::map<int, std::vector<Rinex3Nav::DataGLO>> _navGLO;
    std::map<int, std::vector<Rinex3Nav::DataGAL>> _navGAL;
    std::map<int, std::vector<Rinex3Nav::DataBEI>> _navBEI;

    // * Header information
    Rinex3Nav::HeaderGPS _headerGPS;
    Rinex3Nav::HeaderGLO _headerGLO;
    Rinex3Nav::HeaderGAL _headerGAL;
    Rinex3Nav::HeaderBEI _headerBEI;
};

#endif /* RINEX3NAV_H_ */
