#pragma once

#include "timeutils.h"
#include "stringutils.h"
#include <optional>
#include "enumtypes.h"

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
        DataGPS();
        DataGPS(const DataGPS &) = default;
        DataGPS(DataGPS &&) = default;
        DataGPS &operator=(const DataGPS &) = delete;
        DataGPS &operator=(DataGPS &&other);

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
        DataGLO();
        DataGLO(const DataGLO &) = default;
        DataGLO(DataGLO &&) = default;
        DataGLO &operator=(const DataGLO &) = delete;
        DataGLO &operator=(DataGLO &&other);

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
        DataGAL();
        DataGAL(const DataGAL &) = default;
        DataGAL(DataGAL &&) = default;
        DataGAL &operator=(const DataGAL &) = delete;
        DataGAL &operator=(DataGAL &&other);

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
        // also have 3 spare places

        std::vector<std::optional<double>> toVec();
    };
    //BeiDou
    struct DataBEI {
        DataBEI();
        DataBEI(const DataBEI &) = default;
        DataBEI(DataBEI &&) = default;
        DataBEI &operator=(const DataBEI &) = delete;
        DataBEI &operator=(DataBEI &&other);

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
        double leapSec;
        std::vector<std::string> pgm;

        // Ionospheric alpha and beta constants
        std::vector<double> ialpha;
        std::vector<double> ibeta;

        // Time System correction
        std::vector<double> GPUT;

        void clear();
    };

    struct HeaderGLO {
        double leapSec;
        std::vector<std::string> pgm;

        // Time System correction
        std::vector<double> GLUT;

        void clear();
    };

    struct HeaderGAL {
        double leapSec;
        std::vector<std::string> pgm;

        // Ionospheric constants
        std::vector<double> gal;

        // Time System correction
        std::vector<double> GAUT;

        void clear();
    };

    struct HeaderBEI {
        double leapSec;
        std::vector<std::string> pgm;

        // Ionospheric alpha and beta constants
        std::vector<double> ialpha;
        std::vector<double> ibeta;

        // Time System correction
        std::vector<double> BDUT;

        void clear();
    };

    // Attributes
    // * Header information
    HeaderGPS _headerGPS;
    HeaderGLO _headerGLO;
    HeaderGAL _headerGAL;
    HeaderBEI _headerBEI;

    // * Navigation data structure mapped to PRN for ease of use
    std::map<int, std::vector<Rinex3Nav::DataGPS>> _navGPS;
    std::map<int, std::vector<Rinex3Nav::DataGLO>> _navGLO;
    std::map<int, std::vector<Rinex3Nav::DataGAL>> _navGAL;
    std::map<int, std::vector<Rinex3Nav::DataBEI>> _navBEI;

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

private:
    void readHead(std::ifstream& infile, SatelliteSystem sys);
    std::vector<std::string> getPMGHeader(std::string line);

    template<typename T>
    void readBody(std::map<int, std::vector<T>>& mapData, std::ifstream& infile, SatelliteSystem sys);
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
