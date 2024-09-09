#pragma once

#include "pch.h"
#include "timeutils.h"
#include "stringutils.h"

#ifndef RINEX3NAV_H_
#define RINEX3NAV_H_

class Rinex3Nav
{
public:
    Rinex3Nav();
    ~Rinex3Nav();

    // Data Structures
    //GPS
    struct DataGPS {
        bool isAvailable;
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double clockDrift;
        double clockDriftRate;
        double IODE;
        double Crs;
        double Delta_n;
        double Mo;
        double Cuc;
        double Eccentricity;
        double Cus;
        double Sqrt_a;
        double TOE;
        double Cic;
        double OMEGA;
        double CIS;
        double Io;
        double Crc;
        double Omega;
        double Omega_dot;
        double IDOT;
        double L2_codes_channel;
        double GPS_week;
        double L2_P_data_flag;
        double svAccuracy;
        double svHealth;
        double TGD;
        double IODC;
        double transmission_time;
        double fit_interval;
    };
    //Glonass
    struct DataGLO {
        bool isAvailable;
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double relFreqBias;
        double messageFrameTime;
        double satPosX;
        double satVelX;
        double satAccX;
        double satHealth;
        double satPosY;
        double satVelY;
        double satAccY;
        double freqNum;
        double satPosZ;
        double satVelZ;
        double satAccZ;
        double infoAge;
    };
    //Galileo
    struct DataGAL {
        bool isAvailable;
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;
        double clockBias;
        double clockDrift;
        double clockDriftRate;
        double IOD;
        double Crs;
        double Delta_n;
        double Mo;
        double Cuc;
        double Eccentricity;
        double Cus;
        double Sqrt_a;
        double TOE;
        double Cic;
        double OMEGA;
        double CIS;
        double Io;
        double Crc;
        double Omega;
        double Omega_dot;
        double IDOT;
        double GAL_week;
        double SISA;
        double svHealth;
        double BGD_E5a;
        double BGD_E5b;
        double transmission_time;
        //also have 3 spare places
    };
    //BeiDou!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    struct DataBEI {
        bool isAvailable;
        int PRN;
        std::vector<double> epochInfo;
        double gpsTime;

    };

    // Headers
    struct HeaderGPS {
        // Ionospheric alpha and beta constants
        std::vector<double> ialpha;
        std::vector<double> ibeta;
        // Time System correction
        std::vector<double> GPUT;
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

    // Attributes

    // * Navigation data structure mapped to PRN for ease of use
    std::map<int, std::vector<Rinex3Nav::DataGPS>> _navGPS;
    std::map<int, std::vector<Rinex3Nav::DataGLO>> _navGLO;
    std::map<int, std::vector<Rinex3Nav::DataGAL>> _navGAL;

    // * Header information
    HeaderGPS _headerGPS;
    HeaderGLO _headerGLO;
    HeaderGAL _headerGAL;

    // Functions
    void readGPS(std::ifstream& inputfileGPS); // for separate GPS only navigation files
    void readGLO(std::ifstream& inputfileGLO); // for separate GLO only navigation files
    void readGAL(std::ifstream& inputfileGAL); // for separate GAL only navigation files
    void readMixed(std::ifstream& inputfileMixed); // for mixed navigation files
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGPS> NAV);
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGAL> NAV);
    int EpochMatcher(double obsTime, std::vector<Rinex3Nav::DataGLO> NAV);

    void clear();
};

#endif /* RINEX3NAV_H_ */
