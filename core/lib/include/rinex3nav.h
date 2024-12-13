#pragma once

#ifndef RINEX3NAV_H_
#define RINEX3NAV_H_

#include "timeutils.h"
#include "stringutils.h"
#include <optional>
#include "enumtypes.h"
#include "ireading.h"

namespace rr {
class Rinex3Nav : IReading
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
        // also have 3 spare places

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

    // struct for helping to store info about "TIME SYSTEM CORR"
    struct TimeSysCorr
    {
        std::string type;
        double a0_coef;
        double a1_coef;
        int ref_time;
        int week_number;
        std::optional<std::string> source;
        std::optional<int> utc_id;

        void clear();
    };

    // struct for helping to store info about "IONOSPHERIC CORR"
    struct IonCorr{
        std::string type;
        std::vector<double> params;
        std::optional<std::string> time_mark;
        std::optional<int> sv_id;

        void clear();
    };

    // Headers
    struct HeaderGPS {
        double version;
        std::string type;

        std::vector<std::string> pgm;
        std::vector<std::string> comments;

        // Ionospheric alpha and beta constants
        IonCorr ialpha;
        IonCorr ibeta;

        // Time System correction
        TimeSysCorr GPUT;
        std::vector<double> leap_seconds;

        void clear();
    };

    struct HeaderGLO {
        double version;
        std::string type;

        std::vector<std::string> pgm;
        std::vector<std::string> comments;

        // None Ionospheric corr --!!!
        // Time System correction
        TimeSysCorr GLUT;
        std::vector<double> leap_seconds;

        void clear();
    };

    struct HeaderGAL {
        double version;
        std::string type;

        std::vector<std::string> pgm;
        std::vector<std::string> comments;

        // Ionospheric constants
        IonCorr gal;

        // Time System correction
        TimeSysCorr GAUT;
        std::vector<double> leap_seconds;

        void clear();
    };

    struct HeaderBEI {
        double version;
        std::string type;

        std::vector<std::string> pgm;
        std::vector<std::string> comments;

        // Ionospheric alpha and beta constants
        IonCorr ialpha;
        IonCorr ibeta;

        // Time System correction
        TimeSysCorr BDUT;
        std::vector<double> leap_seconds;

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
    SatelliteSystem curr_sys;

    void readHead(std::ifstream& infile) override;
    std::vector<std::string> getPMGHeader(std::string line);
    TimeSysCorr getTimeSysCorr(std::string line);

    void readBody(std::ifstream& infile) override;
    void saveBlock(std::vector<std::string>& block, SatelliteSystem sys, std::string ID = "");
};

struct ViewNav
{
    ViewNav();
    ViewNav(const Rinex3Nav &other);
    ViewNav &operator=(const Rinex3Nav &other);

    // Attributes
    // * Header information
    Rinex3Nav::HeaderGPS _headerGPS;
    Rinex3Nav::HeaderGLO _headerGLO;
    Rinex3Nav::HeaderGAL _headerGAL;
    Rinex3Nav::HeaderBEI _headerBEI;

    // * Navigation data structure mapped to PRN for ease of use
    std::map<int, std::vector<Rinex3Nav::DataGPS>> _navGPS;
    std::map<int, std::vector<Rinex3Nav::DataGLO>> _navGLO;
    std::map<int, std::vector<Rinex3Nav::DataGAL>> _navGAL;
    std::map<int, std::vector<Rinex3Nav::DataBEI>> _navBEI;
};
}
#endif /* RINEX3NAV_H_ */
