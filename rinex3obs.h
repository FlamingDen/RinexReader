#pragma once

#include "pch.h"
#include "timeutils.h"
#include "stringutils.h"
#include <optional>

#ifndef RINEX3OBS_H_
#define RINEX3OBS_H_

class Rinex3Obs
{
public:
    Rinex3Obs();
    ~Rinex3Obs();

    // DATA STRUCTURES
    // To store important header information
    struct ObsHeaderInfo {
        std::string rinexType;
        std::vector<double> approxPosXYZ;
        std::vector<double> antDeltaHEN;
        std::vector<double> firstObsTime;
        std::vector<double> lastObsTime;
        std::map<std::string, std::vector<std::string>> obsTypes;
    };
    // To store observations in an epoch
    struct ObsEpochInfo {
        std::vector<double> epochRecord;
        std::optional<double> recClockOffset;
        double gpsTime;
        int numSatsGPS;
        int numSatsGLO;
        int numSatsGAL;
        int numSatsBEI;
        std::map<std::string, std::map<int, std::vector<double>>> observations;
        void clear() {
            epochRecord.clear();
            numSatsGAL = NULL;
            numSatsGLO = NULL;
            numSatsGPS = NULL;
            numSatsBEI = NULL;
            observations.clear();
            recClockOffset = NULL;
        }
    };

    // Attributes
    Rinex3Obs::ObsHeaderInfo _Header;
    Rinex3Obs::ObsEpochInfo _EpochObs;

    // * Epoch observations mapped to PRN for ease of use
    std::map<int, std::vector<double>> _obsGPS;
    std::map<int, std::vector<double>> _obsGLO;
    std::map<int, std::vector<double>> _obsGAL;
    std::map<int, std::vector<double>> _obsBEI;

    // * Available observation types (C1C, L1C,...)
    std::vector<std::string> _obsTypesGPS;
    std::vector<std::string> _obsTypesGLO;
    std::vector<std::string> _obsTypesGAL;
    std::vector<std::string> _obsTypesBEI;

    // Functions
    void obsHeader(std::ifstream& infile);
    bool obsEpoch(std::ifstream& infile);
    void clear(Rinex3Obs::ObsEpochInfo& obs);
    void clear(Rinex3Obs::ObsHeaderInfo& header);
    void setObservations(std::map<std::string, std::map<int, std::vector<double>>> observations);
    std::map<int, double> specificObsMapper(std::map<int, std::vector<double>> obsGPS, std::vector<std::string> obsTypes, std::string specificObs);
};

#endif // RINEX3OBS_H
